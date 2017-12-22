#pragma once

#define PLUG_MFR "LukeZeitlin"
#define PLUG_NAME "CombOver2"

#define PLUG_CLASS_NAME CombOver2

#define BUNDLE_MFR "LukeZeitlin"
#define BUNDLE_NAME "CombOver2"

#define PLUG_ENTRY CombOver2_Entry
#define PLUG_VIEW_ENTRY CombOver2_ViewEntry

#define PLUG_ENTRY_STR "CombOver2_Entry"
#define PLUG_VIEW_ENTRY_STR "CombOver2_ViewEntry"

#define VIEW_CLASS CombOver2_View
#define VIEW_CLASS_STR "CombOver2_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'CMTW'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'LUZE'

// ProTools stuff

#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'EFN1', 'EFN2'};
  const int PLUG_TYPE_IDS_AS[2] = {'EFA1', 'EFA2'}; // AudioSuite
#endif

#define PLUG_MFR_PT "LukeZeitlin\nLukeZeitlin\nAcme"
#define PLUG_NAME_PT "CombOver2\nIPEF"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 1

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
instrument determined by PLUG _IS _INST
*/

#define PLUG_CHANNEL_IO "2-2 4-2"
#define PLUG_SC_CHANS 2

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 0
#define AU_MIDI_FX 0

#define PLUG_DOES_STATE_CHUNKS 0

// Unique IDs for each image resource.
#define KNOB_ID 101
#define RAD_ID 102
#define CIR_ID 103
#define BG_ID 104
#define KNOBMETER_ID 105
#define FBSW_ID 106
#define LFOMODE_ID 107

// Image resource locations for this plug.
#define KNOB_FN "resources/img/knob.png"
#define RAD_FN "resources/img/radiobutton1.png"
#define CIR_FN "resources/img/circle.png"
#define BG_FN "resources/img/frameGUI.png"
#define KNOBMETER_FN "resources/img/knobmeter.png"
#define FBSW_FN "resources/img/fbswitch.png"
#define LFOMODE_FN "resources/img/LFOmode.png"

// GUI default dimensions
#define GUI_WIDTH 800
#define GUI_HEIGHT 500

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 "Fx"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/


// Parameters and presets stuff ------------------------------------------------------------------------
#define kNumPrograms 16
#define kNumPublicParams 16
#define kNumPrivateParams 0
  
// Public parameters should go on top and private on the bottom.
// You can freely add or remove parameters from the bottom while preserving the compatibility.
enum EParam
{
  // public:
  kGain = 0,
  kWetPercent,
  kFeedLevelPercent,
  kDelayLengthMS,
  kFeedMode,
  kLFO1Numerator,
  kLFO1Denominator,
  kNumSliders,
  kNumPoints,
  kFiltQ,
  kFiltFreq,
  kLFOSwitch,
  
  
  kMM_02,
  kMM_12,
  kMM_03,
  kMM_13,
  
  kNumParams
  // private:
};
