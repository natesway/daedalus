#include "stdafx.h"

#include <stdio.h>

#include "Core/Memory.h"

#include "Debug/DBGConsole.h"

#include "Graphics/GraphicsContext.h"

#include "HLEGraphics/BaseRenderer.h"
#include "HLEGraphics/TextureCache.h"
#include "HLEGraphics/DLParser.h"
#include "HLEGraphics/DisplayListDebugger.h"

#include "HLEGraphics/GraphicsPlugin.h"

#include "System/Timing.h"

#include "SysGL/GL.h"

#include "SysGL/Interface/imgui.h"
#include "SysGL/Interface/backends/imgui_impl_sdl.h"
#include "SysGL/Interface/backends/imgui_impl_opengl3.h"

extern SDL_Window * gWindow;

EFrameskipValue     gFrameskipValue = FV_DISABLED;
u32                 gVISyncRate     = 1500;
bool                gTakeScreenshot = false;

namespace
{
	//u32					gVblCount = 0;
	u32					gFlipCount = 0;
	//float				gCurrentVblrate = 0.0f;
	float				gCurrentFramerate = 0.0f;
	u64					gLastFramerateCalcTime = 0;
	u64					gTicksPerSecond = 0;

#ifdef DAEDALUS_FRAMERATE_ANALYSIS
	u32					gTotalFrames = 0;
	u64					gFirstFrameTime = 0;
	FILE *				gFramerateFile = NULL;
#endif

static void	UpdateFramerate()
{
#ifdef DAEDALUS_FRAMERATE_ANALYSIS
	gTotalFrames++;
#endif
	gFlipCount++;

	u64			now;
	NTiming::GetPreciseTime( &now );

	if(gLastFramerateCalcTime == 0)
	{
		u64		freq;
		gLastFramerateCalcTime = now;

		NTiming::GetPreciseFrequency( &freq );
		gTicksPerSecond = freq;
	}

#ifdef DAEDALUS_FRAMERATE_ANALYSIS
	if( gFramerateFile == NULL )
	{
		gFirstFrameTime = now;
		gFramerateFile = fopen( "framerate.csv", "w" );
	}
	fprintf( gFramerateFile, "%d,%f\n", gTotalFrames, f32(now - gFirstFrameTime) / f32(gTicksPerSecond) );
#endif

	// If 1 second has elapsed since last recalculation, do it now
	u64		ticks_since_recalc( now - gLastFramerateCalcTime );
	if(ticks_since_recalc > gTicksPerSecond)
	{
		//gCurrentVblrate = float( gVblCount * gTicksPerSecond ) / float( ticks_since_recalc );
		gCurrentFramerate = float( gFlipCount * gTicksPerSecond ) / float( ticks_since_recalc );

		//gVblCount = 0;
		gFlipCount = 0;
		gLastFramerateCalcTime = now;

#ifdef DAEDALUS_FRAMERATE_ANALYSIS
		if( gFramerateFile != NULL )
		{
			fflush( gFramerateFile );
		}
#endif
	}

}
}

class CGraphicsPluginImpl : public CGraphicsPlugin
{
	public:
		CGraphicsPluginImpl();
		~CGraphicsPluginImpl();

				bool		Initialise();

		virtual bool		StartEmulation()		{ return true; }

		virtual void		ViStatusChanged()		{}
		virtual void		ViWidthChanged()		{}
		virtual void		ProcessDList();

		virtual void		UpdateScreen();

		virtual void		RomClosed();

	private:
		u32					LastOrigin;
};



CGraphicsPlugin::~CGraphicsPlugin()
{
}

CGraphicsPluginImpl::CGraphicsPluginImpl()
:	LastOrigin( 0 )
{
}

CGraphicsPluginImpl::~CGraphicsPluginImpl()
{
}

bool CGraphicsPluginImpl::Initialise()
{
	if (!CreateRenderer())
	{
		return false;
	}

	if (!CTextureCache::Create())
	{
		return false;
	}

	if (!DLParser_Initialise())
	{
		return false;
	}

	return true;
}

void CGraphicsPluginImpl::ProcessDList()
{
#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	if (!DLDebugger_Process())
	{
		DLParser_Process();
	}
#else
	DLParser_Process();
#endif
}

	bool ShowFPSonmenu = false;
	bool toggle_fullscreen = false;

void MainMenu(){

	ImGui::NewFrame();
	extern EAudioPluginMode gAudioPluginEnabled;
	if (ImGui::BeginMainMenuBar())
	{
			if (ImGui::BeginMenu("Menu"))
	    	{
	        if (ImGui::MenuItem("Settings"))   {
					}

	        ImGui::EndMenu();
	    }

			if (ImGui::BeginMenu("Debug"))
				{
					if (ImGui::MenuItem("Show FPS"))   {
						if(ShowFPSonmenu == false) {ShowFPSonmenu = true;}
						else if(ShowFPSonmenu == true) {ShowFPSonmenu = false;}
					}

					ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Video Mode"))
				{
					if (ImGui::MenuItem("Fullscreen"))   {
						if (toggle_fullscreen == false) {
							SDL_SetWindowFullscreen(gWindow, SDL_TRUE);
							toggle_fullscreen = true;
						}
						else if (toggle_fullscreen == true) {
							SDL_SetWindowFullscreen(gWindow, SDL_FALSE);
							toggle_fullscreen = false;
						}
					}

					ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Audio mode")) {

				if (ImGui::MenuItem("Disabled")) {
					 gAudioPluginEnabled = APM_DISABLED;
				}

				if (ImGui::MenuItem("Enabled Async")) {
					 gAudioPluginEnabled = APM_ENABLED_ASYNC;
				}

				if (ImGui::MenuItem("Enabled Sync")) {
					gAudioPluginEnabled = APM_ENABLED_SYNC;
				}

				ImGui::EndMenu();

			 }

			if(ShowFPSonmenu == true){
			ImGui::Text("Current FPS %#.1f", gCurrentFramerate);
			}

			ImGui::EndMainMenuBar();
	}


}


void CGraphicsPluginImpl::UpdateScreen()
{
	u32 current_origin = Memory_VI_GetRegister(VI_ORIGIN_REG);

	if (current_origin != LastOrigin)
	{
		UpdateFramerate();

		// FIXME: safe printf
		char string[22];
		sprintf(string, "DaedalusX64");

	SDL_SetWindowTitle(gWindow, string);

//Start ImGui and Render MainMenu
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplSDL2_NewFrame();
MainMenu();
ImGui::Render();
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
ImGui::EndFrame();

		if (gTakeScreenshot)
		{
			CGraphicsContext::Get()->DumpNextScreen();
			gTakeScreenshot = false;
		}

		CGraphicsContext::Get()->UpdateFrame( false );

		LastOrigin = current_origin;
	}
}

void CGraphicsPluginImpl::RomClosed()
{
	DBGConsole_Msg(0, "Finalising GLGraphics");
	ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
	DLParser_Finalise();
	CTextureCache::Destroy();
	DestroyRenderer();
}

class CGraphicsPlugin *	CreateGraphicsPlugin()
{
	DBGConsole_Msg( 0, "Initialising Graphics Plugin [CGL]" );

	CGraphicsPluginImpl * plugin = new CGraphicsPluginImpl;
	if (!plugin->Initialise())
	{
		delete plugin;
		plugin = NULL;
	}

	return plugin;
}
