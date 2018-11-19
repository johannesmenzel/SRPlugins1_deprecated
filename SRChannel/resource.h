#define PLUG_MFR "SheepRaider"
#define PLUG_NAME "SRChannel"

#define PLUG_CLASS_NAME SRChannel

#define BUNDLE_MFR "SheepRaider"
#define BUNDLE_NAME "SRChannel"

#define PLUG_ENTRY SRChannel_Entry
#define PLUG_FACTORY SRChannel_Factory
#define PLUG_VIEW_ENTRY SRChannel_ViewEntry

#define PLUG_ENTRY_STR "SRChannel_Entry"
#define PLUG_VIEW_ENTRY_STR "SRChannel_ViewEntry"

#define VIEW_CLASS SRChannel_View
#define VIEW_CLASS_STR "SRChannel_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00000101
#define VST3_VER_STR "0.1.1"

#define PLUG_COPYRIGHT  "MIT"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'SrCh'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'ShRa'

// ProTools stuff

#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'EFN1', 'EFN2'};
  const int PLUG_TYPE_IDS_AS[2] = {'EFA1', 'EFA2'}; // AudioSuite
#endif

#define PLUG_MFR_PT "SheepRaider\nSheepRaider\nAcme"
#define PLUG_NAME_PT "SRChannel\nIPEF"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 1

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
instrument determined by PLUG _IS _INST
*/

#define PLUG_CHANNEL_IO "2-2 4-2" // 1-1 removed; hannes
#define PLUG_SC_CHANS 2

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 0

#define PLUG_DOES_STATE_CHUNKS 0

// Unique IDs for each image resource.
#define BG_ID			101
#define	LOGO_ID			102

#define BLUEKNOB_ID		110
#define GREENKNOB_ID	111
#define REDKNOB_ID		112
#define ORANGEKNOB_ID	113
#define YELLOWKNOB_ID	114
#define BLACKKNOB_ID	115
#define WHITEKNOB_ID	116
#define ABBEYKNOB_ID	117

#define BUTTON_ID		130

#define FADER_ID		140
#define FADERBG_ID		141


// Image resource locations for this plug.
#define BG_FN			"../SRImages/SrChannel-Background2.png"
#define LOGO_FN			"../SRImages/Logo.png"

#define BLUEKNOB_FN		"../SRImages/SSL_blue_48x48_128.png"
#define GREENKNOB_FN	"../SRImages/SSL_green_48x48_128.png"
#define REDKNOB_FN		"../SRImages/SSL_red_48x48_128.png"
#define ORANGEKNOB_FN	"../SRImages/SSL_orange_48x48_128.png"
#define YELLOWKNOB_FN	"../SRImages/SSL_yellow_48x48_128.png"
#define BLACKKNOB_FN	"../SRImages/SSL_black_48x48_128.png"
#define WHITEKNOB_FN	"../SRImages/SSL_white_48x48_128.png"
#define	ABBEYKNOB_FN	"../SRImages/Abbey_chicken_48x48_128.png"

#define BUTTON_FN		"../SRImages/analogswitch_32x32_2.png"

#define FADER_FN		"../SRImages/Fader_48x96_1.png"
#define FADERBG_FN		"../SRImages/FaderBg_20x600_1.png"


// GUI default dimensions
#define GUI_WIDTH 1024
#define GUI_HEIGHT 768

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "none"
#define MFR_EMAIL "none"
#define EFFECT_TYPE_VST3 "Fx|EQ"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
