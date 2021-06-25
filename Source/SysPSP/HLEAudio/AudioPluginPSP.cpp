/*
Copyright (C) 2003 Azimer
Copyright (C) 2001,2006 StrmnNrmn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

//
//	N.B. This source code is derived from Azimer's Audio plugin (v0.55?)
//	and modified by StrmnNrmn to work with Daedalus PSP. Thanks Azimer!
//	Drop me a line if you get chance :)
//

#include "stdafx.h"
#include <stdio.h>
#include <new>

#include <pspkernel.h>
#include <pspaudiolib.h>
#include <pspaudio.h>

#include "Config/ConfigOptions.h"
#include "Core/CPU.h"
#include "Core/Interrupt.h"
#include "Core/Memory.h"
#include "Core/ROM.h"
#include "Core/RSP_HLE.h"
#include "Debug/DBGConsole.h"
#include "HLEAudio/HLEAudioInternal.h"
#include "HLEAudio/AudioBuffer.h"
#include "HLEAudio/AudioPlugin.h"
#include "SysPSP/Utility/CacheUtil.h"
#include "Core/FramerateLimiter.h"
#include "System/Thread.h"
#include "SysPSP/PRX/MediaEngine/me.h"
#include "SysPSP/Utility/ModulePSP.h"
#include <queue>
#include "Ultra/ultra_sptask.h"

#define RSP_AUDIO_INTR_CYCLES     1
extern u32 gSoundSync;

static const u32	kOutputFrequency = 44100;
static const u32	MAX_OUTPUT_FREQUENCY = kOutputFrequency * 4;

static bool audio_open = false;

// Large kAudioBufferSize creates huge delay on sound //Corn
static const u32	kAudioBufferSize = 1024 * 2; // OSX uses a circular buffer length, 1024 * 1024


class AudioPluginPSP : public CAudioPlugin
{
public:

 AudioPluginPSP();
	virtual ~AudioPluginPSP();
	virtual bool			StartEmulation();
	virtual void			StopEmulation();

	virtual void			DacrateChanged( int system_type );
	virtual void			LenChanged();
	virtual u32				ReadLength() {return 0;}
	virtual EProcessResult	ProcessAList();

	//virtual void SetFrequency(u32 frequency);
	virtual void AddBuffer( u8 * start, u32 length);
	virtual void FillBuffer( Sample * buffer, u32 num_samples);

	virtual void StopAudio();
	virtual void StartAudio();

public:
  CAudioBuffer *		mAudioBufferUncached;

private:
	CAudioBuffer * mAudioBuffer;
	bool mKeepRunning;
	bool mExitAudioThread;
	u32 mFrequency;
	s32 mAudioThread;
	s32 mSemaphore;
	u32 mBufferLenMs;
};

static AudioPluginPSP * ac;

//ME Variables and Functions -> TODO: Move to its own file?
#ifdef DAEDALUS_PSP_USE_ME

bool gLoadedMediaEnginePRX {false};
volatile me_struct *mei;
bool MEStarted = false;
extern std::queue < OSTask > MEQueue;
extern OSTask * p_alistbufferme;
int metimeout = 0;
int MEWorktodo_Samples = 0;
int MEReadytoWork = 0;
std::queue<u32> Address;
std::queue<u32> Length;


//Not working ATM -> code matches what is done on SC. 
int AddBufferME(){
ac->AddBuffer(g_pu8RamBase + Address.front(), Length.front());
}



int MediaEngineFeeder(){

	while(MEStarted == true){

		dcache_wbinv_all();

		if(!MEQueue.empty()){
				memcpy(p_alistbufferme, &MEQueue.front(), sizeof(*p_alistbufferme));
				Audio_Ucode();
				MEQueue.pop();
		}
		
			
		if(!Address.empty()){
					AddBufferME();
					Address.pop();
					Length.pop();
					
		}
		
		
	}


return 0;
}


void Run_Ucode_me(){

	if(MEStarted == false ){

				MEStarted = true;
				sceKernelDcacheWritebackInvalidateAll();
				BeginME( mei, (int)&MediaEngineFeeder, (int)NULL, -1, NULL, -1, NULL);
				

			return;
	}

	return;
}

#endif

void AudioPluginPSP::FillBuffer(Sample * buffer, u32 num_samples)
{
	//sceKernelWaitSema( mSemaphore, 1, nullptr );

	mAudioBufferUncached->Drain( buffer, num_samples );

	//sceKernelSignalSema( mSemaphore, 1 );
}


EAudioPluginMode gAudioPluginEnabled( APM_DISABLED );


AudioPluginPSP::AudioPluginPSP()
:	mKeepRunning (false)
,   mAudioBuffer( nullptr )
,	mFrequency( kOutputFrequency )
,	mSemaphore( sceKernelCreateSema( "AudioPluginPSP", 0, 1, 1, nullptr ) )
,	mBufferLenMs ( 0 )
{
	// Allocate audio buffer with malloc_64 to avoid cached/uncached aliasing
	void * mem = malloc_64( sizeof( CAudioBuffer ) );
	mAudioBuffer = new( mem ) CAudioBuffer( kAudioBufferSize );
	mAudioBufferUncached = (CAudioBuffer*)MAKE_UNCACHED_PTR(mem);
	// Ideally we could just invalidate this range?
	dcache_wbinv_range_unaligned( mAudioBuffer, mAudioBuffer+sizeof( CAudioBuffer ) );
}

AudioPluginPSP::~AudioPluginPSP( )
{
	mAudioBuffer->~CAudioBuffer();
  	free(mAudioBuffer);

  

  	sceKernelDeleteSema(mSemaphore);
  	pspAudioEnd();
}

bool		AudioPluginPSP::StartEmulation()
{
	return true;
}


void	AudioPluginPSP::StopEmulation()
{
    Audio_Reset();
	sceKernelDcacheWritebackInvalidateAll();
  	StopAudio();
    sceKernelDeleteSema(mSemaphore);
    pspAudioEndPre();
    sceKernelDelayThread(100000);
    pspAudioEnd();


}

void	AudioPluginPSP::DacrateChanged( int system_type )
{
	u32 clock = (system_type == ST_NTSC) ? VI_NTSC_CLOCK : VI_PAL_CLOCK;
	u32 dacrate = Memory_AI_GetRegister(AI_DACRATE_REG);
	u32 frequency = clock / (dacrate + 1);

	DBGConsole_Msg(0, "Audio frequency: %d", frequency);
	mFrequency = frequency;
}


void	AudioPluginPSP::LenChanged()
{
		if (!mKeepRunning)
		StartAudio();

	if( gAudioPluginEnabled > APM_DISABLED )
	{
		switch( gAudioPluginEnabled )
	{
	case APM_DISABLED:
		break;

	case APM_ENABLED_ASYNC:
		{
			u32 address = Memory_AI_GetRegister(AI_DRAM_ADDR_REG) & 0xFFFFFF;
			u32	length = Memory_AI_GetRegister(AI_LEN_REG);
			Address.emplace(address);
			Length.emplace(length);
			sceKernelDcacheWritebackInvalidateAll();

		}
		break;

	case APM_ENABLED_SYNC:
		{
		u32 address = Memory_AI_GetRegister(AI_DRAM_ADDR_REG) & 0xFFFFFF;
		u32	length = Memory_AI_GetRegister(AI_LEN_REG);

		AddBuffer( g_pu8RamBase + address, length );
		}
		break;
	}
		
	}
	else
	{
		StopAudio();
	}
}


bool InitialiseMediaEngine()
{

#ifdef DAEDALUS_PSP_USE_ME

	if( CModule::Load("mediaengine.prx") < 0 )	return false;

	mei = (volatile struct me_struct *)malloc_64(sizeof(struct me_struct));
	mei = (volatile struct me_struct *)(MAKE_UNCACHED_PTR(mei));
	sceKernelDcacheWritebackInvalidateAll();

	if (InitME(mei) == 0)
	{
		gLoadedMediaEnginePRX = true;
		return true;
	}
	else
	{
		#ifdef DAEDALUS_DEBUG_CONSOLE
		printf(" Couldn't initialize MediaEngine Instance\n");
		#endif
		return false;
	}
#else
	return false;
#endif

}


EProcessResult	AudioPluginPSP::ProcessAList()
{
	Memory_SP_SetRegisterBits(SP_STATUS_REG, SP_STATUS_HALT);

	EProcessResult	result = PR_NOT_STARTED;

	switch( gAudioPluginEnabled )
	{
		case APM_DISABLED:
			result = PR_COMPLETED;
			break;
		case APM_ENABLED_ASYNC:
			{
			PrepDataUcode();
			sceKernelDcacheWritebackInvalidateAll();
			Run_Ucode_me();
			}
			result = PR_COMPLETED;
			break;

		case APM_ENABLED_SYNC:
			Audio_Ucode();
			result = PR_COMPLETED;
			break;
	}

	return result;
}


void audioCallback( void * buf, unsigned int length, void * userdata )
{
	AudioPluginPSP * ac( reinterpret_cast< AudioPluginPSP * >( userdata ) );
	ac->FillBuffer( reinterpret_cast< Sample * >( buf ), length );

}


void AudioPluginPSP::StartAudio()
{
	if (mKeepRunning == true)
		return;
	
	if(gLoadedMediaEnginePRX == false) {
	InitialiseMediaEngine();
	}

	mKeepRunning = true;

	ac = this;
	
	pspAudioInit();
	
	pspAudioSetChannelCallback( 0, audioCallback, this );

	// Everything OK
	audio_open = true;
}

void AudioPluginPSP::AddBuffer( u8 *start, u32 length )
{

	u32 num_samples = length / sizeof( Sample );

	switch( gAudioPluginEnabled )
	{
	case APM_DISABLED:
		break;

	case APM_ENABLED_ASYNC:
		{
			mAudioBufferUncached->AddSamples( reinterpret_cast< const Sample * >( start ), num_samples, mFrequency, kOutputFrequency );
		}
		break;

	case APM_ENABLED_SYNC:
		{
			mAudioBufferUncached->AddSamples( reinterpret_cast< const Sample * >( start ), num_samples, mFrequency, kOutputFrequency );
		}
		break;
	}
	
}

void AudioPluginPSP::StopAudio()
{
	if (!mKeepRunning)
		return;

	#ifdef DAEDALUS_PSP_USE_ME

	MEStarted = false;

  	while(!MEQueue.empty()){
	  	MEQueue.pop();
  	}

  	while(!Address.empty()){
	 	Address.pop();
	  	Length.pop();
  	}

 	 #endif

	mKeepRunning = false;

	audio_open = false;
}

CAudioPlugin *		CreateAudioPlugin()
{
	return new AudioPluginPSP();
}
