#include "stdafx.h"
#include "HLEAudio/AudioPlugin.h"
#include "Config/ConfigOptions.h"
#include "HLEAudio/audiohle.h"
EAudioPluginMode gAudioPluginEnabled = APM_DISABLED;

CAudioPlugin * CreateAudioPlugin()
{
	return NULL;
}
