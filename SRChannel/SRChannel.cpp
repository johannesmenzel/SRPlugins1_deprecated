#include "SRChannel.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "denormal.h"
//#include "../SRClasses/SRControls.h"
//#include "../SRClasses/SRHelpers.h"
//#include "../SRClasses/SimpleComp.h"
//#include "../SRClasses/SRDynamics.h"
//#include <string>
//#define _USE_MATH_DEFINES
//#include <math.h>
//#define _USE_MATH_DEFINES
//#include <cmath>
//#include <algorithm>
//#include "lice.h"



const int kNumPrograms = 4;									// Here you find the total number of presets defined
//const double METER_ATTACK = 1., METER_DECAY = 0.001;			// This is the global attack and release constants for meters
const double METER_ATTACK = .6, METER_DECAY = .05;			// This is the global attack and release constants for meters
const double halfpi = PI / 2;								// Half pi defined
const double dcoff = 1e-15;									// amount of DC offset added and subtracted respectivly at the beginning and the end of DoubleReplacing loop

const double mEqPassQ_O2_F1 = 0.70710678;					// Setting the Q constants für higher order low- and highpass filter.
const double stQ = mEqPassQ_O2_F1;
const double mEqPassQ_O3_F1 = 1.;
const double mEqPassQ_O4_F1 = 0.54119610;
const double mEqPassQ_O4_F2 = 1.3065630;
//const double mEqPassQ_O5_F1 = 1.;
//const double mEqPassQ_O5_F2 = 1.;
const double mEqPassQ_O6_F1 = 0.51763809;
const double mEqPassQ_O6_F2 = 0.70710678;
const double mEqPassQ_O6_F3 = 1.9318517;
//const double mEqPassQ_O7_F1 = 1.;
//const double mEqPassQ_O7_F2 = 1.;
//const double mEqPassQ_O7_F3 = 1.;
const double mEqPassQ_O8_F1 = 0.50979558;
const double mEqPassQ_O8_F2 = 0.60134489;
const double mEqPassQ_O8_F3 = 0.89997622;
const double mEqPassQ_O8_F4 = 2.5629154;
//const double mEqPassQ_O9_F1 = 1.;
//const double mEqPassQ_O9_F2 = 1.;
//const double mEqPassQ_O9_F3 = 1.;
//const double mEqPassQ_O9_F4 = 1.;
const double mEqPassQ_O10_F1 = 0.50623256;
const double mEqPassQ_O10_F2 = 0.56116312;
const double mEqPassQ_O10_F3 = 0.70710678;
const double mEqPassQ_O10_F4 = 1.1013446;
const double mEqPassQ_O10_F5 = 3.1962266;
const double mEqPassQ_O12_F1 = 0.50431448;
const double mEqPassQ_O12_F2 = 0.54119610;
const double mEqPassQ_O12_F3 = 0.63023621;
const double mEqPassQ_O12_F4 = 0.82133982;
const double mEqPassQ_O12_F5 = 1.3065630;
const double mEqPassQ_O12_F6 = 3.8306488;
const double mEqPassQ_O20_F1 = 0.50154610;
const double mEqPassQ_O20_F2 = 0.51420760;
const double mEqPassQ_O20_F3 = 0.54119610;
const double mEqPassQ_O20_F4 = 0.58641385;
const double mEqPassQ_O20_F5 = 0.65754350;
const double mEqPassQ_O20_F6 = 0.76988452;
const double mEqPassQ_O20_F7 = 0.95694043;
const double mEqPassQ_O20_F8 = 1.3065630;
const double mEqPassQ_O20_F9 = 2.1418288;
const double mEqPassQ_O20_F10 = 6.3727474;

/*##########################################################
Hannes: LAYOUT
##########################################################*/
enum ELayout
{
	kWidth = GUI_WIDTH, kHeight = GUI_HEIGHT,				// Size of plugin
	//kScaleX = kWidth / 23,									// Horizontal spacing of controls
	kScaleX = 50,											// Horizontal spacing of controls
	//kScaleY = kHeight / 17,									// Vertical spacing of controls
	kScaleY = 38,
	//kControlX = kScaleX,									// Horizontal position of the most upper left control
	kControlX = 25,											// Horizontal position of the most upper left control
	kControlY = 100,										// Vertical position of the most upper left control
	kSslKnobFrames = 128,									// Number of frames of the knob images
};

															// All plugins parameters listed here. Take care of the order for the next step
enum EParams
{
  kInputGain = 0,
  kEqHpFreq,
  kEqLpFreq,
  kEqHfGain,
  kEqHfFreq,
  kEqHfQ,
  kEqHfBell,
  kEqHmfGain,
  kEqHmfFreq,
  kEqHmfQ,
  kEqLmfGain,
  kEqLmfFreq,
  kEqLmfQ,
  kEqLfGain,
  kEqLfFreq,
  kEqLfQ,
  kEqLfBell,
  kCompRmsThresh,
  kCompRmsRatio,
  kCompRmsAttack,
  kCompRmsRelease,
  kCompRmsMakeup,
  kCompPeakThresh,
  kCompPeakRatio,
  kCompPeakAttack,
  kCompPeakRelease,
  kCompPeakMakeup,
  kCompPeakRmsRatio,
  kCompDryWet,
  kSaturationAmount,
  kClipperThreshold,
  kOutputGain,
  kPan,
  kPanFreq,
  kLimiterThresh,
  kCompIsParallel,
  kEqBypass,
  kCompBypass,
  kOutputBypass,
  kBypass,
  kSaturationHarmonics,
  kEqHpOrder,
  kEqLpOrder,
  kTestParam1,
  kTestParam2,
  kTestParam3,
  kTestParam4,
  kTestParam5,
  kInputDrive,
  kAgc,
  kCompPeakSidechainFilterFreq,
  kDeesserBottomFreq,
  kDeesserTopFreq,
  kDeesserThresh,
  kDeesserRatio,
  kDeesserAttack,
  kDeesserRelease,
  kDeesserMakeup,
  kCompPeakKneeWidthDb,
  kCompRmsKneeWidthDb,
  kInputBypass,
  kCompPeakIsExtSc,
  kCompRmsIsExrSc,
  kSaturationSkew,
  kNumParams
};

															// All possible controls listed here
enum Knobs {
	SslBlue = 1,
	SslGreen,
	SslRed,
	SslOrange,
	SslYellow,
	SslBlack,
	SslWhite,
	AbbeyChicken,
	Button,
	Fader,
	kNumKnobs
};

															// All parameters possible data types listed here
enum Type {
	typeDouble = 1,
	typeBool,
	typeInt,
	typeEnum,
	kNumType
};

															// Struct object containing possible parameters properties
typedef struct {
	const char* name;				// Name of parameter displayed in host
	const char* shortName;			// Short name for GUI display
	const double defaultVal;		// Plugin loads in this state, return by double click
	const double minVal;			// Minimum value of parameter
	const double maxVal;			// Maximum value  of parameter
	const double stepValue;			// Controls dial steps / accuracy
	const double centerVal;			// Value that void setShapeCentered() will center (knobs middle position)
	const double centerPoint = .5;	// Value WHERE void setShapeCentered() will center centerVal (0.5 means: real middle position)
	const char* label;				// Measuring unit of parameter
	const char* group;				// Parameter group, not supported by every host
	const int Type;					// Data type of parameter
	const int Knobs;				// Used control bitmap
	const int x;					// Horizontal position of control
	const int y;					// Vertical position of control
	const char* labelMin;			// GUI display string of minimum
	const char* labelMax;			// GUI display string of maximum
	const char* labelCtr;			// GUI display string of middle position
	const char* tooltip;			// Tooltip on mouseover
} structParameterProperties;


															/* 
															Actual properties struct.
															Keep in order of the above listet parameters.
															These are constants.
															*/
const structParameterProperties parameterProperties[kNumParams] = {
	//{ "NAME",			"SNAME"	DEF,	MIN,	MAX,	STEP,	CENTER,	CTRPNT,	"LABEL",GROUP ,		TYPE,		KNOB,		kControlX + kScaleX * X,		kControlY + kScaleY * Y,		"LBLMIN", "LBLMAX", "LBLCTR", "TOOLTIP"},
	{ "Input Gain",		"IN",	0.,		-60.,	12.,	0.1,	0.,		10. / 12.,"dB",	"Global",	typeDouble, Fader,		kControlX + kScaleX * 0,		kControlY + kScaleY * 2,		"-60", "12", "0",		"Input Gain is applied before everything else" },
	{ "Highpass Freq",	"HP",	16.,	16.,	350.,	1.,		120.,	.5,		"Hz",	"EQ",		typeDouble, SslWhite,	kControlX + kScaleX * 4,		kControlY + kScaleY * 1,		"16", "350", "120",		"Frequency of the High Pass Filter, turn down to deactivate" },
	{ "Lowpass Freq",	"LP",	22000., 3000.,	22000.,	1.,		5000.,	.5,		"Hz",	"EQ",		typeDouble, SslWhite,	kControlX + kScaleX * 6,		kControlY + kScaleY * 0,		"3k", "22k", "5k",		"Set the frequency of the Low Pass Filter, turn up to deactivate" },
	{ "HF Gain",		"HF",	0.,		-12.,	12.,	.1,		0.,		.5,		"dB",	"EQ",		typeDouble, SslRed,		kControlX + kScaleX * 4,		kControlY + kScaleY * 3,		"-12", "12", "0",		"Gain of the high frequency band" },
	{ "HF Freq",		"FQ",	8000.,	1500.,	16000.,	1.,		8000.,	.5,		"Hz",	"EQ",		typeDouble, SslRed,		kControlX + kScaleX * 6,		kControlY + kScaleY * 4,		"1.5k", "16k", "8k",	"Frequency of the high frequency band" },
	{ "HF Q",			"Q",	stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"EQ",		typeDouble, SslRed,		kControlX + kScaleX * 8,		kControlY + kScaleY * 4,		"W", "N", "",			"Set the Q value of the high frequency band" },
	{ "Hf Bell",		"Bell",	0,		0,		1,		1,		0.5,	.5,		"",		"EQ",		typeBool,	Button,		kControlX + kScaleX * 6,		kControlY + kScaleY * 3,		"SLF", "BLL", "",		"Switches the high frequency band between bell and shelf" },
	{ "HMF Gain",		"HMF",	0.,		-12.,	12.,	0.1,	0.,		.5,		"dB",	"EQ",		typeDouble, SslBlue,	kControlX + kScaleX * 4,		kControlY + kScaleY * 5,		"-12", "12", "0",		"Gain of the upper mid frequency band" },
	{ "HMF Freq",		"FQ",	3000.,	600.,	7000.,	1.,		3000.,	.5,		"Hz",	"EQ",		typeDouble, SslBlue,	kControlX + kScaleX * 6,		kControlY + kScaleY * 6,		"600", "7k", "3k",		"Frequency of the upper mid frequency band" },
	{ "HMF Q",			"Q",	stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"EQ",		typeDouble, SslBlue,	kControlX + kScaleX * 4,		kControlY + kScaleY * 7,		"W", "N", "",			"Set the Q value of the upper mid frequency band" },
	{ "LMF Gain",		"LMF",	0.,		-12.,	12.,	0.1,	0.,		.5,		"dB",	"EQ",		typeDouble, SslGreen,	kControlX + kScaleX * 4,		kControlY + kScaleY * 9,		"-12", "12", "0",		"Gain of the lower mid frequency band" },
	{ "LMF Freq",		"FQ",	1000.,	200.,	2500.,	1.,		1000.,	.5,		"Hz",	"EQ",		typeDouble, SslGreen,	kControlX + kScaleX * 6,		kControlY + kScaleY * 10,		"200", "2.5k", "1k",	"Frequency of the lower mid frequency band" },
	{ "LMF Q",			"Q",	stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"EQ",		typeDouble, SslGreen,	kControlX + kScaleX * 4,		kControlY + kScaleY * 11,		"W", "N", "",			"Set the Q value of the lower mid frequency band" },
	{ "LF Gain",		"LF",	0.,		-12.,	12.,	0.1,	0.,		.5,		"dB",	"EQ",		typeDouble, SslBlack,	kControlX + kScaleX * 4,		kControlY + kScaleY * 13,		"-12", "12", "0",		"Gain of the low frequency band" },
	{ "LF Freq",		"FQ",	200.,	30.,	450.,	1.,		200.,	.5,		"Hz",	"EQ",		typeDouble, SslBlack,	kControlX + kScaleX * 6,		kControlY + kScaleY * 12,		"30", "450", "200",		"Set the frequency of the low frequency band" },
	{ "LF Q",			"Q",	stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"EQ",		typeDouble, SslBlack,	kControlX + kScaleX * 8,		kControlY + kScaleY * 12,		"W", "N", "",			"Set the Q value of the low frequency band" },
	{ "Lf Bell",		"Bell",	0,		0,		1,		0.1,	0.5,	.5,		"",		"EQ",		typeBool,	Button,		kControlX + kScaleX * 6,		kControlY + kScaleY * 14,		"SLF", "BLL", "",		"Switches the low frequency band between bell and shelf" },
	{ "RMS Thresh",		"THR",	0.,		-40.,	0.,		0.1,	-20.,	.5,		"dB",	"Compressor",typeDouble, SslWhite,	kControlX + kScaleX * 8,		kControlY + kScaleY * 0,		"-40", "0", "-20",		"Threshold of RMS Compressor" },
	{ "RMS Ratio",		"RAT",	3.,		0.5,	20.,	0.01,	3.,		.5,		":1",	"Compressor",typeDouble, SslWhite,	kControlX + kScaleX * 10,		kControlY + kScaleY * 1,		".5:1", "20:1", "3:1",	"Ratio of RMS Compressor" },
	{ "RMS Attack",		"ATT",	20.,	10.,	200.,	0.01,	20.,	.5,		"ms",	"Compressor",typeDouble, SslWhite,	kControlX + kScaleX * 8,		kControlY + kScaleY * 2,		"10", "200", "20",		"Attack of RMS Compressor" },
	{ "RMS Release",	"REL",	200.,	100.,	5000.,	1.,		200.,	.5,		"ms",	"Compressor",typeDouble, SslWhite,	kControlX + kScaleX * 10,		kControlY + kScaleY * 3,		"100", "5k", "200",		"Release of RMS Compressor" },
	{ "RMS Makeup",		"MK",	0.,		0.,		40.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslBlack,	kControlX + kScaleX * 8,		kControlY + kScaleY * 4,		"0", "40", "10",		"Makeup gain of RMS Compressor" },
	{ "Peak Thresh",	"THR",	0.,		-40.,	0.,		0.1,	-20.,	.5,		"dB",	"Compressor",typeDouble, SslWhite,	kControlX + kScaleX * 8,		kControlY + kScaleY * 8,		"-40", "0", "-20",		"Threshold of Peak Compressor" },
	{ "Peak Ratio",		"RAT",	3.,		0.5,	20.,	0.01,	3.,		.5,		":1",	"Compressor",typeDouble, SslWhite,	kControlX + kScaleX * 10,		kControlY + kScaleY * 9,		".5:1", "20:1", "3:1",	"Ratio of Peak Compressor" },
	{ "Peak Attack",	"ATT",	20.,	0.01,	200.,	0.01,	20.,	.5,		"ms",	"Compressor",typeDouble, SslWhite,	kControlX + kScaleX * 8,		kControlY + kScaleY * 10,		"0.01", "200", "20",	"Attack of Peak Compressor" },
	{ "Peak Release",	"REL",	200.,	1.,		2000.,	0.01,	200.,	.5,		"ms",	"Compressor",typeDouble, SslWhite,	kControlX + kScaleX * 10,		kControlY + kScaleY * 11,		"1", "2k", "200",		"Release of Peak Compressor" },
	{ "Peak Makeup",	"MK",	0.,		0.,		40.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslBlack,	kControlX + kScaleX * 8,		kControlY + kScaleY * 12,		"0", "40", "10",		"Makeup Gain of Peak Compressor" },
	{ "RMS/Peak Ratio", "CR",	50.,	0.,		100.,	0.1,	50.,	.5,		"%",	"Compressor",typeDouble, SslBlack,	kControlX + kScaleX * 8,		kControlY + kScaleY * 16,		"0", "100", "50",		"Mix the signal of the RMS and Peak Compressor" },
	{ "Dry/Wet",		"MIX",	100.,	0.,		100.,	1.,		50.,	.5,		"%",	"Compressor",typeDouble, SslBlack,	kControlX + kScaleX * 10,		kControlY + kScaleY * 16,		"0", "100", "50",		"Do parallel compression by dialing in the uncompressed signal" },
	{ "Sat Amount",		"AMT",	0.,		0.,		100.,	0.01,	10.,	.5,		"%",	"Input",	typeDouble, SslOrange,	kControlX + kScaleX * 2,		kControlY + kScaleY * 0,		"0", "100", "50",		"Amount of Saturation" },
	{ "Clipper",		"CLP",	0.,		0.,		99.,	0.01,	50.,	.5,		"%",	"Output",	typeDouble, SslYellow,	kControlX + kScaleX * 16,		kControlY + kScaleY * 0,		"0", "100", "50",		"Amount of the Output Clipper (cuts amplitutde)" },
	{ "Output Gain",	"OUT",	0.,		-60.,	12.,	0.1,	0.,		10. / 12.,"dB",	"Global",	typeDouble, Fader,		kControlX + kScaleX * 16,		kControlY + kScaleY * 2,		"-60", "12", "0",		"Output Gain" },
	{ "Pan",			"PAN",	0.,		-100.,	100.,	1.,		0.,		.5,		"%",	"Output",	typeDouble, SslBlue,	kControlX + kScaleX * 14,		kControlY + kScaleY * 1,		"L", "R", "C",			"Pan" },
	{ "Pan Freq",		"PNF",	150.,	20.,	1000.,	1.,		150.,	.5,		"Hz",	"Output",	typeDouble, SslRed,		kControlX + kScaleX * 14,		kControlY + kScaleY * 3,		"20", "1k", "200",		"Frequencies below crossover will be not affected by panner" },
	{ "Limiter",		"LMT",	10.,	-30.,	10.,	0.1,	0.,		.5,		"dB",	"Output",	typeDouble, SslOrange,	kControlX + kScaleX * 14,		kControlY + kScaleY * 5,		"-30", "10", "0",		"Thresold of Output Limiter" },
	{ "Comp Ser/Par",	"Par",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,		kControlX + kScaleX * 9,		kControlY + kScaleY * 16,		"SER", "PAR", "",		"RMS and Peak Compressor can be run serial or parallel" },
	{ "EQ Bypass",		"EQ Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",		typeBool,	Button,		kControlX + kScaleX * 4 - 32,	kControlY + kScaleY * 0 - 36,	"ACT", "BYP", "",		"Bypass Equalizer Section" },
	{ "Comp Bypass",	"Comp Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",typeBool,	Button,		kControlX + kScaleX * 8 - 32,	kControlY + kScaleY * 0 - 36,	"ACT", "BYP", "",		"Bypass Compressor Section" },
	{ "Output Bypass",	"Out Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,		kControlX + kScaleX * 14 - 32,	kControlY + kScaleY * 0 - 36,	"ACT", "BYP", "",		"Bypass Output Section" },
	{ "Bypass",			"Byp",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,		kControlX + kScaleX * 0 - 32,	kControlY + kScaleY * -1 - 36,	"ACT", "BYP", "",		"Bypass Plugin" },
	{ "Harmonics",		"HRM",	50.,	0.,		100.,	.01,	50.,	.5,		"%",	"Input",	typeDouble, SslBlue,	kControlX + kScaleX * 2,		kControlY + kScaleY * 2,		"Even", "Odd", "Mix",	"Dial in even harmonics by turning the knob counter-clockwise" },
	{ "HP Order",		"HPO",	2,		1,		9,		1,		5,		.5,		"dB/oct", "EQ",		typeInt,	AbbeyChicken, kControlX + kScaleX * 2,		kControlY + kScaleY * 14,		"6", "120", "36",		"Order of the Highpass Filter or filter slope" },
	{ "LP Order",		"LPO",	2,		1,		9,		1,		5,		.5,		"dB/oct", "EQ",		typeInt,	AbbeyChicken, kControlX + kScaleX * 2,		kControlY + kScaleY * 16,		"6", "120", "36",		"NOT WORKING - Order of the Lowpass Filter or filter slope" },
	{ "TestParam 1",	"T1",	0.2,	0.2,	5.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 4,		0,								"0", "1", "0.5",		"NOT WORKING - Generic control for development tests" },
	{ "TestParam 2",	"T2",	0.2,	0.2,	5.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 6,		0,								"0", "1", "0.5",		"NOT WORKING - Generic control for development tests" },
	{ "TestParam 3",	"T3",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 8,		0,								"0", "1", "0.5",		"NOT WORKING - Generic control for development tests" },
	{ "TestParam 4",	"T4",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 10,		0,								"0", "1", "0.5",		"NOT WORKING - Generic control for development tests" },
	{ "TestParam 5",	"T5",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 12,		0,								"0", "1", "0.5",		"NOT WORKING - Generic control for development tests" },
	{ "Sat Drive",		"DRV",	0.,		0.,		60.,	0.1,	30.,	.5,		"dB",	"Input",	typeDouble, SslRed,		kControlX + kScaleX * 0,		kControlY + kScaleY * 0,		"0", "60", "30",		"Saturation input drive, hits the saturation module harder" },
	{ "Auto Gain Comp",	"AGC",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Output",	typeBool,	Button,		kControlX + kScaleX * 14,		kControlY + kScaleY * 7,		"AGC", "", "",			"Automatic Gain Conpensation. Click to adjust output volume to input volume" },
	{ "Peak SC Filter",	"SCF",	16.,	16.,	5000.,	1.,		1000.,	.5,		"Hz",	"Compressor",typeDouble, SslBlue,	kControlX + kScaleX * 8,		kControlY + kScaleY * 14,		"16", "5k", "1k",		"Frequency of the Compressors sidechain highpass filter" },
	{ "Deesser Bottom",	"DSB",	6000.,	16.,	22000.,	1.,		7000.,	.5,		"Hz",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 0,		"16", "22k", "7k",		"NOT WORKING - Deessers affects frequencies above" },
	{ "Deesser Top",	"DST",	7000.,	16.,	22000.,	1.,		7000.,	.5,		"Hz",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 2,		"16", "22k", "7k",		"NOT WORKING - Deessers affects frequencies below" },
	{ "Deesser Thresh",	"DST",	0.,		-100.,	0.,		0.1,	-50.,	.5,		"dB",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 4,		"-100", "0", "-50",		"NOT WORKING - Deessers Threshold" },
	{ "Deesser Ratio",	"DSR",	3.,		0.5,	20.,	0.01,	3.,		.5,		":1",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 6,		".5:1", "20:1", "3:1",	"NOT WORKING - Deessers Ratio" },
	{ "Deesser Attack",	"DSA",	20.,	0.01,	200.,	0.01,	20.,	.5,		"ms",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 8,		"0.01", "200", "20",	"NOT WORKING - Deessers Attack" },
	{ "Deesser Release","DSD",	200.,	1.,		2000.,	0.01,	200.,	.5,		"ms",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 10,		"1", "2k", "200",		"NOT WORKING - Deessers Release" },
	{ "Deesser Makeup",	"DSM",	0.,		0.,		40.,	0.1,	10.,	.5,		"dB",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 12,		"0", "40", "10",		"NOT WORKING - Deessers Makeup" },
	{ "Peak Knee",		"SKN",	0.,		0.,		30.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslGreen,	kControlX + kScaleX * 10,		kControlY + kScaleY * 13,		"0", "30", "10",		"Peak Compressors width of the soft knee" },
	{ "RMS Knee",		"SKN",	0.,		0.,		30.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslGreen,	kControlX + kScaleX * 10,		kControlY + kScaleY * 5,		"0", "30", "10",		"RMS Compressors width of the soft knee" },
	{ "Input Bypass",	"In Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,		kControlX + kScaleX * 0 - 32,	kControlY + kScaleY * 0 - 36,	"ACT", "BYP", "",		"Bypass Input Section" },
	{ "Peak Ext SC",	"Ext",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,		kControlX + kScaleX * 10,		kControlY + kScaleY * 15,		"INT", "EXT", "",		"External sidechain for Peak Compressor. Use tracks input channel 3/4" },
	{ "RMS Ext SC",		"Ext",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,		kControlX + kScaleX * 8,		kControlY + kScaleY * 6,		"INT", "EXT", "",		"External sidechain for RMS Compressor. Use tracks input channel 3/4" },
	{ "Sat Skew",		"SKW",	0,		-100.,	100.,	0.01,	0.,		.5,		"%",	"Input",	typeDouble, SslOrange,	kControlX + kScaleX * 2,		kControlY + kScaleY * 4,		"-100", "100", "0",		"Saturations positive/negative skewness. Distorts waveform" }

	//{ "NAME",			"SNAME"	DEF,	MIN,	MAX,	STEP,	CENTER,	CTRPNT,	"LABEL",GROUP ,		TYPE,		KNOB,		kControlX + kScaleX * X,		kControlY + kScaleY * Y,		"LBLMIN", "LBLMAX", "LBLCTR", "TOOLTIP"},
};


/*##########################################################
						CONSTRUCTOR
##########################################################*/

SRChannel::SRChannel(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
	mInputPeakMeterValue1(0.),
	mInputPeakMeterValue2(0.),
	mRmsGrMeterValue(0.),
	mPeakGrMeterValue(0.),
	mOutputPeakMeterValue1(0.),
	mOutputPeakMeterValue2(0.),
	mInputPeakMeterPreviousValue1(0.),
	mInputPeakMeterPreviousValue2(0.),
	mOutputPeakMeterPreviousValue1(0.),
	mOutputPeakMeterPreviousValue2(0.),
	mInputPeakMeterTimeConst1(0.),
	mInputPeakMeterTimeConst2(0.),
	mOutputPeakMeterTimeConst1(0.),
	mOutputPeakMeterTimeConst2(0.),
	mOutputVuMeterValue1(0.),
	mOutputVuMeterValue2(0.),
	mOutputVuMeterSos1(0.),
	mOutputVuMeterSos2(0.)
{
  TRACE;


														  /*
														  First we call the parameter initialization function. We might want the presets and graphics created after parameter initialization.
														  We call methods of the Reset() function just not to have them typed here, because they have to be initialized on plugins start
														  For example the filters and dynamic processing classes
														  */
  CreateParams();
  CreatePresets();
  CreateGraphics();
  InitGUI();
  Reset();
}

/*##########################################################
						USER METHODS
##########################################################*/

void SRChannel::CreateParams() {
	for (int i = 0; i < kNumParams; i++) {											// We iterate through ALL parameters

		IParam* param = GetParam(i);												// ... for which we temporally create a pointer "param"
		const structParameterProperties &properties = parameterProperties[i];		// ... and a variable "properties" pointing at the current parameters properties
		/*
		switch (i) {																// this is the old 
				// Enum Parameters:
			case mOsc1Waveform:
			case mOsc2Waveform:
				param->InitEnum(properties.name,
					Oscillator::OSCILLATOR_MODE_SAW,
					Oscillator::kNumOscillatorModes);
				// For VST3:
				param->SetDisplayText(0, properties.name);
				break;
			case mLFOWaveform:
				param->InitEnum(properties.name,
					Oscillator::OSCILLATOR_MODE_TRIANGLE,
					Oscillator::kNumOscillatorModes);
				// For VST3:
				param->SetDisplayText(0, properties.name);
				break;
			case mFilterMode:
				param->InitEnum(properties.name,
					Filter::FILTER_MODE_LOWPASS,
					Filter::kNumFilterModes);
				break;
			default:
				break;
			*/

		switch(properties.Type) {							// each type of parameter needs another initialization method

			case typeBool:									// Create parameters of type boolean.
				param->InitBool(
					properties.name,
					bool(properties.defaultVal),
					properties.label,
					properties.group
				);
				break;

			case typeDouble:								// Create parameters of type double.
				param->InitDouble(
					properties.name,
					properties.defaultVal,
					properties.minVal,
					properties.maxVal,
					properties.stepValue,
					properties.label,
					properties.group, 
					SRPlugins::SRHelpers::SetShapeCentered(						// We calculate the controls shape.
						properties.minVal,
						properties.maxVal,
						properties.centerVal,
						properties.centerPoint
					)
				);

				break;

			case typeInt:
				param->InitInt(
					properties.name,
					int(properties.defaultVal),
					int(properties.minVal),
					int(properties.maxVal),
					properties.label,
					properties.group
				);
				break;

			default:										// Just in case there are paramters of other types.
				break;
		}
		
		OnParamChange(i);									// Run OnParamChange() once for each parameter to initialize member variables and so on.

	}
}


void SRChannel::CreatePresets() {
															// These have to be created when Parameters are complete, so work in progress.
															// Following are original procedures to make default presets
//	MakeDefaultPreset((char *) "Full Reset", kNumPrograms);
//	MakePreset("Default", kNumPrograms);
															// Here's the beginning of the real stuff.
	MakeDefaultPreset("Default", 1);
	MakePreset("Kick", 0., 30., 8500., 3., 8000., .707, 0, 4.5, 5600., .2, -6., 320., .707, 4., 57., 2., 1, 0., 1., 20., 200., 0., -33., 3., 25., 150., 4.5, 100., 70., 10., 0., -.3, 0., 150., -4., 0);
	MakePreset("Vocals", 0., 90., 22000., 5.7, 12200., .707, 0, 2.6, 5000., .3, -2., 478., 3.8, 2., 133., .707, 0, -25., 3., 20., 320., 3., -21., 19., .18, 80., 1.6, 50., 100., 50., 0., -1.5, 0., 150., -5., 0);
	MakePreset("Mix Bus", 0., 30., 22000., 4., 14000., .707, 0, 3.5, 5700., .2, -2., 610., 3., 2., 60., .707, 1, -20., 3., 55., 450., 7.5, -13., 15., .8, 85., 2.5, 50., 80., 20., 0., -4., 0., 150., -.5, 1);
//	MakePreset("NAME", InGain, HpFreq, LpFreq, HfGain, HfFreq, HfQ, HfBell, HmfGain, HmfFreq, HmfQ, LmfGain, LmfFreq, LmfQ, LfGain, LfFreq, LfQ, LfBell, RmsThresh, RmsRatio, RmsAttack, RmsRelease, RmsMakeup, PeakThresh, PeakRatio, PeakAttack, PeakRelease, PeakMakeup, PeakRmsRatio, DryWet, Sat, Clp, OutGain);
}

void SRChannel::GrayOutControls()
{
	if (GetGUI()) {
		for (int i = 0; i < kNumParams; i++) {
			bool grayout;

			(mBypass == 1 && parameterProperties[i].group != "Global") ? grayout = true
			: (mInputBypass == 1 && parameterProperties[i].group == "Input") ? grayout = true
			: (mCompBypass == 1 && (parameterProperties[i].group == "Compressor" || parameterProperties[i].group == "Deesser")) ? grayout = true
			: (mEqBypass == 1 && parameterProperties[i].group == "EQ") ? grayout = true
			: (mOutputBypass == 1 && parameterProperties[i].group == "Output") ? grayout = true
			: (mCompIsParallel == 0 && i == kCompPeakRmsRatio) ? grayout = true
			: (i == kTestParam1 
				|| i == kTestParam2 
				|| i == kTestParam3 
				|| i == kTestParam4 
				|| i == kTestParam5 
				|| i == kEqLpOrder 
				|| i == kSaturationSkew 
				|| i == kDeesserAttack 
				|| i == kDeesserRelease 
				|| i == kDeesserThresh 
				|| i == kDeesserRatio 
				|| i == kDeesserBottomFreq
				|| i == kDeesserMakeup
				|| i == kDeesserTopFreq) ? grayout = true
			: grayout = false;

			GetGUI()->GrayOutControl(i, grayout);
		}
	}
}



void SRChannel::CreateGraphics() {
	// Define colors and text properties
		// Colors
	IColor colorBg = IColor(50, 154, 168, 178);
	IColor colorBgShadow = IColor(50, 0, 0, 0);
	IColor colorFg = IColor(255, 154, 168, 178);
	IColor colorLabel = IColor(121, 255, 255, 255);
	IColor colorMeterFg = IColor(150, 255, 120, 0);
	IColor colorMeterBg = IColor(50, 0, 0, 0);

		// Text Properties
	IText textFg = IText(14, &colorFg, "", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityClearType);
	IText textKnobRingMinProp = IText(14, &colorLabel, "Century Gothic", IText::kStyleNormal, IText::kAlignFar, 0, IText::kQualityClearType);
	IText textKnobRingMaxProp = IText(14, &colorLabel, "Century Gothic", IText::kStyleNormal, IText::kAlignNear, 0, IText::kQualityClearType);
	IText textKnobRingCtrProp = IText(14, &colorLabel, "Century Gothic", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityClearType);
	IText textKnobLabelProp = IText(18, &colorLabel, "Century Gothic", IText::kStyleBold, IText::kAlignFar, 0, IText::kQualityClearType);
	IText textButtonLabelProp = IText(16, &colorLabel, "Century Gothic", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityClearType);
	IText textSectionLabelProp = IText(24, &colorLabel, "Century Gothic", IText::kStyleBold, IText::EAlign::kAlignCenter, 0, IText::kQualityClearType);
	IText textPresetLabelProp = IText(18, &colorLabel, "Century Gothic", IText::kStyleBold, IText::EAlign::kAlignNear, 0, IText::kQualityClearType);

	// Loading Graphics and connect to parameters
		// Basic GUI Properties
	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BG_ID, BG_FN);
	IBitmap bitmapLogo = pGraphics->LoadIBitmap(LOGO_ID, LOGO_FN);
	pGraphics->AttachControl(new IBitmapControl(this, kWidth - 300, 0, &bitmapLogo));
	pGraphics->AttachControl(new ITextControl(this, IRECT(kWidth - 300, 0, kWidth, 20), &textPresetLabelProp, "v." VST3_VER_STR));
	
	// Background Shadowing
	//pGraphics->AttachControl(new IPanelControl(this, IRECT(kControlX + kScaleX * 0 - 32, bitmapLogo.H, kControlX + kScaleX * 4 - 8, kHeight), &colorBgShadow)); // Input Stage
	//pGraphics->AttachControl(new IPanelControl(this, IRECT(kControlX + kScaleX * 5 - 32, bitmapLogo.H, kControlX + kScaleX * 9 - 8, kHeight), &colorBgShadow)); // EQ Section
	//pGraphics->AttachControl(new IPanelControl(this, IRECT(kControlX + kScaleX * 10 - 32, bitmapLogo.H, kControlX + kScaleX * 16 - 8, kHeight), &colorBgShadow)); // Comp Section
	//pGraphics->AttachControl(new IPanelControl(this, IRECT(kControlX + kScaleX * 17 - 32, bitmapLogo.H, kControlX + kScaleX * 21 - 8, kHeight), &colorBgShadow)); // Output Stage
	//pGraphics->AttachControl(new SRControls::ISectionRect(this, IRECT(kControlX + kScaleX * 0 - 32, bitmapLogo.H, kControlX + kScaleX * 4 - 8, kHeight), &colorBgShadow, &textFg, "Equalizer"));

	// Preset Menu
	pGraphics->AttachControl(new SRPlugins::SRControls::IPresetMenu(this, IRECT(kControlX + kScaleX * 0 - 32, 10, kControlX + kScaleX * 4 - 8, 28), &textPresetLabelProp));

	// Meters
		// Peak and GR
		IRECT meter = IRECT(kWidth - 60, kControlY, kWidth, kHeight);
		cInputPeakMeter1 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 59, bitmapLogo.H, kWidth - 51, kHeight), colorMeterBg, colorMeterFg));
		cInputPeakMeter2 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 49, bitmapLogo.H, kWidth - 41, kHeight), colorMeterBg, colorMeterFg));
		cRmsGrMeter = pGraphics->AttachControl(new SRPlugins::SRControls::IGrMeterVert(this, IRECT(kWidth - 39, bitmapLogo.H, kWidth - 31, kHeight), colorMeterBg, colorMeterFg));
		cPeakGrMeter = pGraphics->AttachControl(new SRPlugins::SRControls::IGrMeterVert(this, IRECT(kWidth - 29, bitmapLogo.H, kWidth - 21, kHeight), colorMeterBg, colorMeterFg));
		cOuputPeakMeter1 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 19, bitmapLogo.H, kWidth - 11, kHeight), colorMeterBg, colorMeterFg));
		cOutputPeakMeter2 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 9, bitmapLogo.H, kWidth - 1, kHeight), colorMeterBg, colorMeterFg));
		cOutputVuMeter1 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 79, bitmapLogo.H, kWidth - 71, kHeight), colorMeterBg, colorMeterFg));
		cOutputVuMeter2 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 69, bitmapLogo.H, kWidth - 61, kHeight), colorMeterBg, colorMeterFg));


		// Peak Meter Labels
		for (int measureDb = 0; measureDb <= 20; measureDb++) {
			std::string measureStr = std::to_string(-measureDb * 3);
			const char *measureChar = measureStr.c_str();
			pGraphics->AttachControl(new ITextControl(this, 
				IRECT(
					kWidth - 60, 
					int(bitmapLogo.H + measureDb * .05 * (kHeight - bitmapLogo.H) - 7), 
					kWidth, 
					int(bitmapLogo.H + measureDb * .05 * (kHeight - bitmapLogo.H) + 7)
				), 
				&textKnobRingCtrProp, 
				measureChar));
		}

		// Biquad Frequency Response
		//cFreqRespGraph = pGraphics->AttachControl(new SRControls::IFreqRespGraph(this, IRECT(kControlX + kScaleX * 15 - 32, kControlY + kScaleY * 8, kControlX + kScaleX * 19 - 8, kHeight), meterFreqRespValues, 64));




	// Load bitmamps
	IBitmap knobSslBlue = pGraphics->LoadIBitmap(BLUEKNOB_ID, BLUEKNOB_FN, kSslKnobFrames);
	IBitmap knobSslGreen = pGraphics->LoadIBitmap(GREENKNOB_ID, GREENKNOB_FN, kSslKnobFrames);
	IBitmap knobSslRed = pGraphics->LoadIBitmap(REDKNOB_ID, REDKNOB_FN, kSslKnobFrames);
	IBitmap knobSslOrange = pGraphics->LoadIBitmap(ORANGEKNOB_ID, ORANGEKNOB_FN, kSslKnobFrames);
	IBitmap knobSslYellow = pGraphics->LoadIBitmap(YELLOWKNOB_ID, YELLOWKNOB_FN, kSslKnobFrames);
	IBitmap knobSslBlack = pGraphics->LoadIBitmap(BLACKKNOB_ID, BLACKKNOB_FN, kSslKnobFrames);
	IBitmap knobSslWhite = pGraphics->LoadIBitmap(WHITEKNOB_ID, WHITEKNOB_FN, kSslKnobFrames);
	IBitmap knobAbbeyChicken = pGraphics->LoadIBitmap(ABBEYKNOB_ID, ABBEYKNOB_FN, kSslKnobFrames);
	IBitmap buttonSimple = pGraphics->LoadIBitmap(BUTTON_ID, BUTTON_FN, 2, false);
	IBitmap faderGain = pGraphics->LoadIBitmap(FADER_ID, FADER_FN, 1);

	size_t cControlMatrixSize = kNumParams;
	cControlMatrix.resize(cControlMatrixSize);

	for (int i = 0; i < kNumParams; i++) {
		const structParameterProperties& properties = parameterProperties[i];
		IBitmap *knob;									// We're pointing at the type of knob we want to add
		
		switch (properties.Knobs) {						// "knob" is gonna be a pointer to IBitmap
		case SslBlue: knob = &knobSslBlue; break;
		case SslGreen: knob = &knobSslGreen; break;
		case SslRed: knob = &knobSslRed; break;
		case SslOrange: knob = &knobSslOrange; break;
		case SslYellow: knob = &knobSslYellow; break;
		case SslBlack: knob = &knobSslBlack; break;
		case SslWhite: knob = &knobSslWhite; break;
		case AbbeyChicken: knob = &knobAbbeyChicken; break;
		case Button: knob = &buttonSimple; break;
		case Fader: knob = &faderGain; break;
		}

		IRECT knobspace = IRECT(properties.x, properties.y, properties.x + 2 * kScaleX, properties.y + 2 * kScaleY);

		int knobwidth = knob->frameWidth();
		int knobheight = knob->frameHeight();

		switch (properties.Type) {						// Testing for type of parameter, bools get buttons, doubles get knobs a.s.o.a.s.f.
	
		case typeBool:
			pGraphics->AttachControl(new ITextControl(this, IRECT(properties.x + 32, properties.y + 0, properties.x + 64, properties.y + 16), &textKnobRingMaxProp, properties.labelMax));
			pGraphics->AttachControl(new ITextControl(this, IRECT(properties.x + 32, properties.y + 16, properties.x + 64, properties.y + 32), &textKnobRingMaxProp, properties.labelMin));
			
				switch (i)
				{
				case kAgc:
					cControlMatrix.at(i) = pGraphics->AttachControl(new IContactControl(this, properties.x, properties.y, i, knob));
					break;
				default:
					cControlMatrix.at(i) = pGraphics->AttachControl(new ISwitchControl(this, properties.x, properties.y, i, knob));
					break;
				}
			break;

		case typeDouble:
		case typeInt:
			switch (i)									// We've got some special cases in parameters of type double
			{
			case kEqHfQ:
			case kEqLfQ:
				break;									// ... like parameters for which we don't want to have a control drawn

			case kInputGain:							// ... or faders
			case kOutputGain:
				//pGraphics->AttachControl(new IBitmapControl(this, (properties.x + faderGain.W * .5 - bitmapFaderBg.W * .5), (properties.y), &bitmapFaderBg));
				pGraphics->AttachControl(new IPanelControl(this, IRECT((int(properties.x + faderGain.W * .5) - 4), (properties.y), (int(properties.x + faderGain.W * .5) + 4), kHeight - 10), &COLOR_BLACK));
				cControlMatrix.at(i) = pGraphics->AttachControl(new IFaderControl(this, properties.x, properties.y, kHeight - properties.y - 10, i, knob, kVertical, true));
				for (int measureDb = 0; measureDb <= 12; measureDb++) {
					std::string measureStr = std::to_string((-measureDb + 2) * 6);
					char const *measureChar = measureStr.c_str();
					//				pGraphics->AttachControl(new ITextControl(this, IRECT(properties.x + 48, properties.y + faderGain.H * .5 + measureDb * .0833333333 * (kHeight - properties.y - 10 - faderGain.H) - 7, properties.x + 64, properties.y + faderGain.H * .5 + measureDb * .083333333 * (kHeight - properties.y - 10 - faderGain.H) + 7), &textKnobRingCtrProp, measureChar));
					pGraphics->AttachControl(new ITextControl(this, IRECT(properties.x + 48, int(properties.y + faderGain.H * .5 + pow(measureDb * .0833333333, GetParam(i)->GetShape()) * (kHeight - properties.y - 10 - faderGain.H) - 7), properties.x + 64, int(properties.y + faderGain.H * .5 + pow(measureDb * .0833333333, GetParam(i)->GetShape()) * (kHeight - properties.y - 10 - faderGain.H) + 7)), &textKnobRingCtrProp, measureChar));
				}
				break;


			default:									// ... but most of them get a nice rotary control
														// Label for minimum value
				pGraphics->AttachControl(new ITextControl(this, IRECT(
					properties.x,
					properties.y + knobheight - 12,
					properties.x + int(.15 * knobwidth),
					properties.y + knobheight + 2
				), &textKnobRingMinProp, properties.labelMin));
														// Label for maximum value
				pGraphics->AttachControl(new ITextControl(this, IRECT(
					properties.x + int(.85 * knobwidth),
					properties.y + knobheight - 12,
					properties.x + knobwidth,
					properties.y + knobheight + 2
				), &textKnobRingMaxProp, properties.labelMax));
														// Label for center value
				pGraphics->AttachControl(new ITextControl(this, IRECT(
					properties.x + int(knobwidth * .5) - 16,
					properties.y - 14,
					properties.x + int(knobwidth * .5) + 16,
					properties.y
				), &textKnobRingCtrProp, properties.labelCtr));
														// Label for short parameter name
				pGraphics->AttachControl(new ITextControl(this, IRECT(
					properties.x,
					properties.y - 10,
					properties.x + int(.15 * knobwidth),
					properties.y + 6
				), &textKnobLabelProp, properties.shortName));
														// Actual rotary control
				cControlMatrix.at(i) = pGraphics->AttachControl(new SRPlugins::SRControls::IKnobMultiControlText(this, IRECT(
					properties.x,
					properties.y,
					properties.x + knobwidth,
					properties.y + knobheight + textFg.mSize
				), i, knob, &textFg, properties.label, kVertical, 4.));
				break;
			}
		}
	}
	AttachGraphics(pGraphics);
														// Gray out controls that don't work by now.
}


void SRChannel::InitGUI() {
	if (GetGUI()) {
		GetGUI()->EnableTooltips(true);
		//GetGUI()->UpdateTooltips();
		//GetGUI()->AssignParamNameToolTips();
		GetGUI()->GetControl(cOutputVuMeter1)->SetTooltip("NOT WORKING - Real VU Meter of left output channel");
		GetGUI()->GetControl(cOutputVuMeter2)->SetTooltip("NOT WORKING - Real VU Meter of right output channel");
		GetGUI()->GetControl(cInputPeakMeter1)->SetTooltip("VU Meter of left input channel");
		GetGUI()->GetControl(cInputPeakMeter2)->SetTooltip("VU Meter of right input channel");
		GetGUI()->GetControl(cRmsGrMeter)->SetTooltip("Gain reduction of RMS compressor");
		GetGUI()->GetControl(cPeakGrMeter)->SetTooltip("Gain reduction of peak compressor");
		GetGUI()->GetControl(cOuputPeakMeter1)->SetTooltip("VU Meter of left output channel");
		GetGUI()->GetControl(cOutputPeakMeter2)->SetTooltip("VU Meter of right output channel");
		for (int i = 0; i < kNumParams; i++) {
			GetGUI()->GetControl(cControlMatrix.at(i))->SetTooltip(parameterProperties[i].tooltip);
			//GetGUI()->ShowControlBounds(true);
		}
	}
}

void SRChannel::InitBiquad() {
	mSampleRate = GetSampleRate();
	fEqHpFilterOnepoleL.setFc(mEqHpFreq);
	fEqHpFilter1L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter2L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter3L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter4L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter5L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter6L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter7L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter8L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter9L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter10L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqLpFilter1L.setFilter(SRPlugins::SRFilters::biquad_lowpass, mEqLpFreq / mSampleRate, stQ, 0., mSampleRate);
//	fEqHpFilter1L.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mEqHpFreq / mSampleRate, 0., 0., mSampleRate);
//	fEqLpFilter1L.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mEqLpFreq / mSampleRate, 0., 0., mSampleRate);
	fEqHfFilterL.setFilter(SRPlugins::SRFilters::biquad_highshelf, mEqHfFreq / mSampleRate, mEqHfQ, mEqHfGain, mSampleRate);
	fEqHmfFilterL.setFilter(SRPlugins::SRFilters::biquad_peak, mEqHmfFreq / mSampleRate, mEqHmfQ, mEqHmfGain, mSampleRate);
	fEqLmfFilterL.setFilter(SRPlugins::SRFilters::biquad_peak, mEqLmfFreq / mSampleRate, mEqLmfQ, mEqLmfGain, mSampleRate);
	fEqLfFilterL.setFilter(SRPlugins::SRFilters::biquad_lowshelf, mEqLfFreq / mSampleRate, mEqLfQ, mEqLfGain, mSampleRate);

	fDcBlockerL.setFc(10. / mSampleRate);

	//fSatHpFilterL.setFilter(SRPlugins::SRFilters::biquad_highpass, 20 / mSampleRate, stQ, 0., mSampleRate);
	//fSatLfFilterL.setFilter(SRPlugins::SRFilters::biquad_lowshelf, 60 / mSampleRate, .5, mSatLfGain, mSampleRate);
	//fSatMfFilterL.setFilter(SRPlugins::SRFilters::biquad_peak, 2200 / mSampleRate, .5, mSatMfGain, mSampleRate);
	//fSatHfFilterL.setFilter(SRPlugins::SRFilters::biquad_highshelf, 15000 / mSampleRate, .5, mSatHfGain, mSampleRate);
	//fSatLpFilterL.setFilter(SRPlugins::SRFilters::biquad_lowpass, 20000 / mSampleRate, stQ, 0., mSampleRate);

	fEqHpFilterOnepoleR.setFc(mEqHpFreq);
	fEqHpFilter1R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter2R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter3R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter4R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter5R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter6R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter7R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter8R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter9R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqHpFilter10R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
	fEqLpFilter1R.setFilter(SRPlugins::SRFilters::biquad_lowpass, mEqLpFreq / mSampleRate, stQ, 0., mSampleRate);
//	fEqHpFilter1R.setFilter(iir_linkwitz_highpass, mEqHpFreq / mSampleRate, 0., 0., mSampleRate);
//	fEqLpFilter1R.setFilter(iir_linkwitz_lowpass, mEqLpFreq / mSampleRate, 0., 0., mSampleRate);
	fEqHfFilterR.setFilter(SRPlugins::SRFilters::biquad_highshelf, mEqHfFreq / mSampleRate, mEqHfQ, mEqHfGain, mSampleRate);
	fEqHmfFilterR.setFilter(SRPlugins::SRFilters::biquad_peak, mEqHmfFreq / mSampleRate, mEqHmfQ, mEqHmfGain, mSampleRate);
	fEqLmfFilterR.setFilter(SRPlugins::SRFilters::biquad_peak, mEqLmfFreq / mSampleRate, mEqLmfQ, mEqLmfGain, mSampleRate);
	fEqLfFilterR.setFilter(SRPlugins::SRFilters::biquad_lowshelf, mEqLfFreq / mSampleRate, mEqLfQ, mEqLfGain, mSampleRate);

	fDcBlockerR.setFc(10. / mSampleRate);

	//fSatHpFilterR.setFilter(SRPlugins::SRFilters::biquad_highpass, 20 / mSampleRate, 0.70710678, 0., mSampleRate);
	//fSatLfFilterR.setFilter(SRPlugins::SRFilters::biquad_lowshelf, 60 / mSampleRate, .5, mSatLfGain, mSampleRate);
	//fSatMfFilterR.setFilter(SRPlugins::SRFilters::biquad_peak, 2200 / mSampleRate, .5, mSatMfGain, mSampleRate);
	//fSatHfFilterR.setFilter(SRPlugins::SRFilters::biquad_highshelf, 15000 / mSampleRate, .5, mSatHfGain, mSampleRate);
	//fSatLpFilterR.setFilter(SRPlugins::SRFilters::biquad_lowpass, 20000 / mSampleRate, 0.70710678, 0., mSampleRate);
}

void SRChannel::InitCompPeak() {
	mSampleRate = GetSampleRate();
	//fCompressorPeak.setSampleRate(mSampleRate);
	//fCompressorPeak.setRatio(mCompPeakRatio);
	//fCompressorPeak.setThresh(mCompPeakThresh);
	//fCompressorPeak.setAttack(mCompPeakAttack);
	//fCompressorPeak.setRelease(mCompPeakRelease);
	fCompressorPeak.setCompressor(mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease, mCompPeakSidechainFilterFreq, mCompPeakKneeWidthDb, mSampleRate);
	fCompressorPeak.initRuntime();
}

void SRChannel::InitCompRms() {
	mSampleRate = GetSampleRate();
	fCompressorRms.setSampleRate(mSampleRate);
	fCompressorRms.setRatio(mCompRmsRatio);
	fCompressorRms.setThresh(mCompRmsThresh);
	fCompressorRms.setAttack(mCompRmsAttack);
	fCompressorRms.setRelease(mCompRmsRelease);
	fCompressorRms.setKnee(mCompRmsKneeWidthDb);
	fCompressorRms.setWindow(300.);
	fCompressorRms.initSidechainFilter(100.);
	fCompressorRms.initRuntime();
}

void SRChannel::InitLimiter() {
	mSampleRate = GetSampleRate();
	fLimiter.setSampleRate(mSampleRate);
	fLimiter.setAttack(1.);
	fLimiter.setRelease(100.);
	fLimiter.setThresh(mLimiterThresh);
	fLimiter.initRuntime();
}

void SRChannel::InitSafePan() {
	mSampleRate = GetSampleRate();
	fSafePanHpL.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
	fSafePanLpL.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
	fSafePanHpR.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
	fSafePanLpR.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
}

void SRChannel::InitDeesser() {
	mSampleRate = GetSampleRate();
	fDeesserBottomLpFilterL.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mDeesserBottomFreq / mSampleRate, 0., 0., mSampleRate);
	fDeesserBottomLpFilterR.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mDeesserBottomFreq / mSampleRate, 0., 0., mSampleRate);
	fDeesserMidHpFilterL.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mDeesserBottomFreq / mSampleRate, 0., 0., mSampleRate);
	fDeesserMidHpFilterR.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mDeesserBottomFreq / mSampleRate, 0., 0., mSampleRate);
	fDeesserMidLpFilterL.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mDeesserTopFreq / mSampleRate, 0., 0., mSampleRate);
	fDeesserMidLpFilterR.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mDeesserTopFreq / mSampleRate, 0., 0., mSampleRate);
	fDeesserTopHpFilterL.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mDeesserTopFreq / mSampleRate, 0., 0., mSampleRate);
	fDeesserTopHpFilterR.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mDeesserTopFreq / mSampleRate, 0., 0., mSampleRate);

	//fDeesserBottomLpFilterL.setFilter(SRPlugins::SRFilters::biquad_lowpass, mDeesserBottomFreq / mSampleRate, stQ, 0., mSampleRate);
	//fDeesserBottomLpFilterR.setFilter(SRPlugins::SRFilters::biquad_lowpass, mDeesserBottomFreq / mSampleRate, stQ, 0., mSampleRate);
	//fDeesserMidHpFilterL.setFilter(SRPlugins::SRFilters::biquad_highpass, mDeesserBottomFreq / mSampleRate, stQ, 0., mSampleRate);
	//fDeesserMidHpFilterR.setFilter(SRPlugins::SRFilters::biquad_highpass, mDeesserBottomFreq / mSampleRate, stQ, 0., mSampleRate);
	//fDeesserMidLpFilterL.setFilter(SRPlugins::SRFilters::biquad_lowpass, mDeesserTopFreq / mSampleRate, stQ, 0., mSampleRate);
	//fDeesserMidLpFilterR.setFilter(SRPlugins::SRFilters::biquad_lowpass, mDeesserTopFreq / mSampleRate, stQ, 0., mSampleRate);
	//fDeesserTopHpFilterL.setFilter(SRPlugins::SRFilters::biquad_highpass, mDeesserTopFreq / mSampleRate, stQ, 0., mSampleRate);
	//fDeesserTopHpFilterR.setFilter(SRPlugins::SRFilters::biquad_highpass, mDeesserTopFreq / mSampleRate, stQ, 0., mSampleRate);

	fDeesser.setCompressor(mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, 20, 5.0, mSampleRate);
}

void SRChannel::InitMeter() {
	mSampleRate = GetSampleRate();
	fOutputVuMeterEnvelope1.setAttack(300), fOutputVuMeterEnvelope2.setAttack(300);
	fOutputVuMeterEnvelope1.setRelease(300), fOutputVuMeterEnvelope2.setRelease(300);
	fOutputVuMeterEnvelope1.setSampleRate(mSampleRate), fOutputVuMeterEnvelope2.setSampleRate(mSampleRate);
	fOutputVuMeterEnvelopeDetector1.setSampleRate(mSampleRate), fOutputVuMeterEnvelopeDetector2.setSampleRate(mSampleRate);
	fOutputVuMeterEnvelopeDetector1.setTc(5), fOutputVuMeterEnvelopeDetector2.setTc(5);
}

void SRChannel::InitSaturation() {
	fInputSaturation.setSaturation(SRPlugins::typeMusicDSP, mInputDrive, mSaturationAmount, mSaturationHarmonics, false, mSaturationSkew, 1.);
}

void SRChannel::InitExtSidechain()
{
	if (GetAPI() == kAPIVST2) // for VST2 we name individual outputs
	{
		SetInputLabel(0, "In Left");
		SetInputLabel(1, "In Right");
		SetInputLabel(2, "ExtSC Left");
		SetInputLabel(3, "ExtSC Right");
		SetOutputLabel(0, "Out Left");
		SetOutputLabel(1, "Out Right");
	}
	else // for AU and VST3 we name buses
	{
		SetInputBusLabel(0, "Input");
		SetInputBusLabel(1, "Ext SC");
		SetOutputBusLabel(0, "Output");
	}
}

/*
void SRChannel::CalculateFreqResp() {
	for (int i = 0; i <= sizeof(meterFreqRespValues) - 1; i++) {
		meterFreqRespValues[i] = fEqHmfFilterL.getFreqResp(double(i / (sizeof(meterFreqRespValues) - 1)) * mSampleRate);
	}
}
*/

SRChannel::~SRChannel() {}

/*##########################################################
						PROCESS
##########################################################*/

void SRChannel::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
	// Mutex is already locked for us.

	// for platform safe implementation check wdl sidechain example again and research for resource.h channel-io implementation
	//bool in1ic = IsInChannelConnected(0);
	//bool in2ic = IsInChannelConnected(1);
	//bool in3ic = IsInChannelConnected(2);
	//bool in4ic = IsInChannelConnected(3);

	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* sc1 = inputs[2];
	double* sc2 = inputs[3];
	double* out1 = outputs[0];
	double* out2 = outputs[1];

	mInputPeakMeterValue1 = 0., mInputPeakMeterValue2 = 0.;
	mRmsGrMeterValue = 0., mPeakGrMeterValue = 0.;
	mOutputPeakMeterValue1 = 0., mOutputPeakMeterValue2 = 0.;


	// Begin Processing per Frame

	for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2) {

		if (mBypass == 1) {
			*out1 = *in1;
			*out2 = *in2;
		}
		else {
			// Adding DC Off
			*out1 = *in1 + dcoff;
			*out2 = *in2 + dcoff;


			// Input Gain
			if (mInputGain != 1.) {
				*out1 *= mInputGain;
				*out2 *= mInputGain;
			}

			// Input Meter
			mInputPeakMeterValue1 = IPMAX(mInputPeakMeterValue1, fabs(*out1));
			mInputPeakMeterValue2 = IPMAX(mInputPeakMeterValue2, fabs(*out2));

			// Fill circular buffer with input gain values
			//circularBuffer[1][circularBufferPointer] = *out1;
			//circularBuffer[2][circularBufferPointer] = *out2;
			circularBufferInL[circularBufferPointer] = *out1;
			circularBufferInR[circularBufferPointer] = *out2;
			
			
			if (mInputBypass != 1) {

				//// Upsample
				//// double y = mOverSampler.Process(x, [](double input) { return std::tanh(input); })

				fInputSaturation.process(*out1, *out2);

				//// Downsample

			}

			// EQ
			if (mEqBypass != 1) {
				if (mEqHpFreq > 16.) {
					if (mEqHpOrder == 1 || mEqHpOrder == 3) {
						*out1 = fEqHpFilterOnepoleL.process(*out1);
						*out2 = fEqHpFilterOnepoleR.process(*out2);
					}
					if (mEqHpOrder >= 2) {
						*out1 = fEqHpFilter1L.process(*out1);
						*out2 = fEqHpFilter1R.process(*out2);
					}
					if (mEqHpOrder >= 4) {
						*out1 = fEqHpFilter2L.process(*out1);
						*out2 = fEqHpFilter2R.process(*out2);
					}
					if (mEqHpOrder >= 5) {
						*out1 = fEqHpFilter3L.process(*out1);
						*out2 = fEqHpFilter3R.process(*out2);
					}
					if (mEqHpOrder >= 6) {
						*out1 = fEqHpFilter4L.process(*out1);
						*out2 = fEqHpFilter4R.process(*out2);
					}
					if (mEqHpOrder >= 7) {
						*out1 = fEqHpFilter5L.process(*out1);
						*out2 = fEqHpFilter5R.process(*out2);
					}
					if (mEqHpOrder >= 8) {
						*out1 = fEqHpFilter6L.process(*out1);
						*out2 = fEqHpFilter6R.process(*out2);
					}
					if (mEqHpOrder >= 9) {
						*out1 = fEqHpFilter7L.process(*out1);
						*out2 = fEqHpFilter7R.process(*out2);
						*out1 = fEqHpFilter8L.process(*out1);
						*out2 = fEqHpFilter8R.process(*out2);
						*out1 = fEqHpFilter9L.process(*out1);
						*out2 = fEqHpFilter9R.process(*out2);
						*out1 = fEqHpFilter10L.process(*out1);
						*out2 = fEqHpFilter10R.process(*out2);
					}
				}
				if (mEqLpFreq < 22000.) {
					*out1 = fEqLpFilter1L.process(*out1);
					*out2 = fEqLpFilter1R.process(*out2);
				}
				if (mEqLfGain != 0.) {
					*out1 = fEqLfFilterL.process(*out1);
					*out2 = fEqLfFilterR.process(*out2);
				}
				if (mEqLmfGain != 0.) {
					*out1 = fEqLmfFilterL.process(*out1);
					*out2 = fEqLmfFilterR.process(*out2);
				}
				if (mEqHmfGain != 0.) {
					*out1 = fEqHmfFilterL.process(*out1);
					*out2 = fEqHmfFilterR.process(*out2);
				}
				if (mEqHfGain != 0.) {
					*out1 = fEqHfFilterL.process(*out1);
					*out2 = fEqHfFilterR.process(*out2);
				}
			}


			// Deesser
			/*vDeesserLowSignalL = *out1;
			vDeesserLowSignalR = *out2;
			vDeesserMidSignalL = *out1;
			vDeesserMidSignalR = *out2;
			vDeesserHighSignalL = *out1;
			vDeesserHighSignalR = *out2;

			vDeesserLowSignalL = fDeesserBottomLpFilterL.process(vDeesserLowSignalL);
			vDeesserLowSignalR = fDeesserBottomLpFilterR.process(vDeesserLowSignalR);
			vDeesserMidSignalL = fDeesserMidHpFilterL.process(fDeesserMidLpFilterL.process(vDeesserMidSignalL));
			vDeesserMidSignalR = fDeesserMidHpFilterR.process(fDeesserMidLpFilterR.process(vDeesserMidSignalR));
			vDeesserHighSignalL = fDeesserTopHpFilterL.process(vDeesserHighSignalL);
			vDeesserHighSignalR = fDeesserTopHpFilterR.process(vDeesserHighSignalR);

			fDeesser.process(vDeesserMidSignalL, vDeesserMidSignalR);

			*out1 = (vDeesserLowSignalL - vDeesserMidSignalL + vDeesserHighSignalL) * mDeesserMakeup;
			*out2 = (vDeesserLowSignalR - vDeesserMidSignalR + vDeesserHighSignalR) * mDeesserMakeup;*/

			//	 Simple Compressor
			if (mCompBypass != 1) {
				double vCompDry1 = *out1;
				double vCompDry2 = *out2;

				if (mCompIsParallel == 1) {
					double vCompRmsIn1 = *out1;
					double vCompPeakIn1 = *out1;
					double vCompRmsIn2 = *out2;
					double vCompPeakIn2 = *out2;

					if (mCompRmsRatio != 1. && mCompRmsThresh != 0.) {
						(mCompRmsIsExtSc != 1) ? fCompressorRms.process(vCompRmsIn1, vCompRmsIn2) : fCompressorRms.process(vCompRmsIn1, vCompRmsIn2, *sc1, *sc2);
					}

					if (mCompPeakRatio != 1. && mCompPeakThresh != 0.) {
						(mCompPeakIsExtSc != 1) ? fCompressorPeak.process(vCompPeakIn1, vCompPeakIn2) : fCompressorPeak.process(vCompPeakIn1, vCompPeakIn2, *sc1, *sc2);
					}

					if ((mCompRmsRatio != 1 && mCompRmsThresh != 0) || (mCompPeakRatio != 1 && mCompPeakThresh != 0)) {
						*out1 = (1 - mCompPeakRmsRatio) * vCompRmsIn1 * mCompRmsMakeup * mCompRmsAutoMakeup + mCompPeakRmsRatio * vCompPeakIn1 * mCompPeakMakeup * mCompPeakAutoMakeup;
						*out2 = (1 - mCompPeakRmsRatio) * vCompRmsIn2 * mCompRmsMakeup * mCompRmsAutoMakeup + mCompPeakRmsRatio * vCompPeakIn2 * mCompPeakMakeup * mCompPeakAutoMakeup;
					}
				}
				else {
					if (mCompRmsRatio != 1. && mCompRmsThresh != 0.) {
						(mCompRmsIsExtSc != 1) ? fCompressorRms.process(*out1, *out2) : fCompressorRms.process(*out1, *out2, *sc1, *sc2);
						*out1 *= mCompRmsMakeup * mCompRmsAutoMakeup;
						*out2 *= mCompRmsMakeup * mCompRmsAutoMakeup;
					}

					if (mCompPeakRatio != 1. && mCompPeakThresh != 0.) {
						(mCompPeakIsExtSc != 1) ? fCompressorPeak.process(*out1, *out2) : fCompressorPeak.process(*out1, *out2, *sc1, *sc2);
						*out1 *= mCompPeakMakeup * mCompPeakAutoMakeup;
						*out2 *= mCompPeakMakeup * mCompPeakAutoMakeup;
					}
				}

				// Dry/Wet
				if (mCompDryWet != 1.) {
					*out1 = (mCompDryWet * *out1) + (vCompDry1 * (1. - mCompDryWet));
					*out2 = (mCompDryWet * *out2) + (vCompDry2 * (1. - mCompDryWet));
				}

			}

			// Pan
			if (mPan != .5) {
				vSafePanLowSignal1 = *out1;
				vSafePanLowSignal2 = *out2;
				vSafePanHighSignal1 = *out1;
				vSafePanHighSignal2 = *out2;

				vSafePanLowSignal1 = fSafePanLpL.process(vSafePanLowSignal1);
				vSafePanLowSignal2 = fSafePanLpR.process(vSafePanLowSignal2);
				vSafePanHighSignal1 = fSafePanHpL.process(vSafePanHighSignal1);
				vSafePanHighSignal2 = fSafePanHpR.process(vSafePanHighSignal2);

				//				*out1 = -vSafePanHighSignal1 * cos(mPan * (halfpi)) + vSafePanLowSignal1;
				//				*out2 = -vSafePanHighSignal2 * sin(mPan * (halfpi)) + vSafePanLowSignal2;
				if (mPan < .5) {
					*out1 = vSafePanHighSignal1 - vSafePanLowSignal1;
					*out2 = vSafePanHighSignal2 * sqrt(mPan) * 1.41 - vSafePanLowSignal2;
				}
				else {
					*out1 = vSafePanHighSignal1 * sqrt(1 - mPan) * 1.41 - vSafePanLowSignal1;
					*out2 = vSafePanHighSignal2 - vSafePanLowSignal2;
				}
				//*out1 = vSafePanHighSignal1 * sqrt(1 - mPan) * 1.41 - vSafePanLowSignal1;
				//*out2 = vSafePanHighSignal2 * sqrt(mPan) * 1.41 - vSafePanLowSignal2;
			}



			if (mOutputBypass != 1) {
				// Clipper
				
				if (mClipperThreshold < 1.) {
					*out1 = (fmin(fabs(*out1), mClipperThreshold) / sqrt(mClipperThreshold)) * (fabs(*out1) / *out1);
					*out2 = (fmin(fabs(*out2), mClipperThreshold) / sqrt(mClipperThreshold)) * (fabs(*out2) / *out2);
				}
				

				// Limiter
				if (mLimiterThresh != 10.) {
					fLimiter.process(*out1, *out2);
				}

				// Soft Limiter
				if (*out1 > 1.) {
					*out1 = (1. - 4 / (*out1 + (4 - 1.))) * 4 + 1.;
				}
				else {
					if (*out1 < -1.) {
						*out1 = (1. + 4 / (*out1 - (4 - 1.))) * -4 - 1.;
					}
				}

				if (*out2 > 1.) {
					*out2 = (1. - 4 / (*out2 + (4 - 1.))) * 4 + 1.;
				}
				else {
					if (*out2 < -1.) {
						*out2 = (1. + 4 / (*out2 - (4 - 1.))) * -4 - 1.;
					}
				}
			}

			// Fill circular buffer with output gain values
			//circularBuffer[3][circularBufferPointer] = *out1;
			//circularBuffer[4][circularBufferPointer] = *out2;
			circularBufferOutL[circularBufferPointer] = *out1;
			circularBufferOutR[circularBufferPointer] = *out2;

			// Output Gain
			if (mOutputGain != 1.) {
				*out1 *= mOutputGain;
				*out2 *= mOutputGain;
			}

			*out1 = fDcBlockerL.process(*out1);
			*out2 = fDcBlockerR.process(*out2);
			*out1 -= dcoff;
			*out2 -= dcoff;

		}



		// Output Meter +  GR Meter
		if (mCompBypass != 1 && mCompRmsRatio != 1. && mCompRmsThresh != 0. && mBypass != 1) {
			mRmsGrMeterValue += (-fabs(fCompressorRms.getGrDb()) + 60) / 60;
		} else {
			mRmsGrMeterValue += 1.;
		}

		if (mCompBypass != 1 && mCompPeakRatio != 1. && mCompPeakThresh != 0. && mBypass != 1) {
			mPeakGrMeterValue += (-fabs(fCompressorPeak.getGrDb()) + 60) / 60;
		} else {
			mPeakGrMeterValue += 1.;
		}

		mOutputPeakMeterValue1 = IPMAX(mOutputPeakMeterValue1, fabs(*out1));
		mOutputPeakMeterValue2 = IPMAX(mOutputPeakMeterValue2, fabs(*out2));

		fOutputVuMeterEnvelopeDetector1.run(fabs(*out1), mOutputVuMeterSos1);
		fOutputVuMeterEnvelopeDetector2.run(fabs(*out2), mOutputVuMeterSos2);
		fOutputVuMeterEnvelope1.run(AmpToDB(mOutputVuMeterSos1), mOutputVuMeterValue1);
		fOutputVuMeterEnvelope2.run(AmpToDB(mOutputVuMeterSos2), mOutputVuMeterValue2);


		(circularBufferPointer >= circularBufferLenght - 1) ? circularBufferPointer = 0 : circularBufferPointer++;

	}	// End Processing per Frame


	// Begin Processing per Framesize

    mInputPeakMeterTimeConst1 = (mInputPeakMeterValue1 < mInputPeakMeterPreviousValue1 ? METER_DECAY : METER_ATTACK);
	mInputPeakMeterTimeConst2 = (mInputPeakMeterValue2 < mInputPeakMeterPreviousValue2 ? METER_DECAY : METER_ATTACK);
	mInputPeakMeterValue1 = mInputPeakMeterValue1 * mInputPeakMeterTimeConst1 + mInputPeakMeterPreviousValue1 * (1.0 - mInputPeakMeterTimeConst1);
	mInputPeakMeterValue2 = mInputPeakMeterValue2 * mInputPeakMeterTimeConst2 + mInputPeakMeterPreviousValue2 * (1.0 - mInputPeakMeterTimeConst2);
	mInputPeakMeterPreviousValue1 = mInputPeakMeterValue1;
	mInputPeakMeterPreviousValue2 = mInputPeakMeterValue2;
	mInputPeakMeterValue1 = (AmpToDB(mInputPeakMeterValue1) + 60) / 60;
	mInputPeakMeterValue2 = (AmpToDB(mInputPeakMeterValue2) + 60) / 60;

	mOutputPeakMeterTimeConst1 = (mOutputPeakMeterValue1 < mOutputPeakMeterPreviousValue1 ? METER_DECAY : METER_ATTACK);
	mOutputPeakMeterTimeConst2 = (mOutputPeakMeterValue2 < mOutputPeakMeterPreviousValue2 ? METER_DECAY : METER_ATTACK);
	mOutputPeakMeterValue1 = mOutputPeakMeterValue1 * mOutputPeakMeterTimeConst1 + mOutputPeakMeterPreviousValue1 * (1.0 - mOutputPeakMeterTimeConst1);
	mOutputPeakMeterValue2 = mOutputPeakMeterValue2 * mOutputPeakMeterTimeConst2 + mOutputPeakMeterPreviousValue2 * (1.0 - mOutputPeakMeterTimeConst2);
	mOutputPeakMeterPreviousValue1 = mOutputPeakMeterValue1;
	mOutputPeakMeterPreviousValue2 = mOutputPeakMeterValue2;
	mOutputPeakMeterValue1 = (AmpToDB(mOutputPeakMeterValue1) + 60) / 60;
	mOutputPeakMeterValue2 = (AmpToDB(mOutputPeakMeterValue2) + 60) / 60;

	
	mRmsGrMeterValue /= (double)nFrames;
	mPeakGrMeterValue /= (double)nFrames;

	if (GetGUI())
	{
		GetGUI()->SetControlFromPlug(cInputPeakMeter1, mInputPeakMeterValue1);
		GetGUI()->SetControlFromPlug(cInputPeakMeter2, mInputPeakMeterValue2);
		GetGUI()->SetControlFromPlug(cRmsGrMeter, mRmsGrMeterValue);
		GetGUI()->SetControlFromPlug(cPeakGrMeter, mPeakGrMeterValue);
		GetGUI()->SetControlFromPlug(cOuputPeakMeter1, mOutputPeakMeterValue1);
		GetGUI()->SetControlFromPlug(cOutputPeakMeter2, mOutputPeakMeterValue2);
		GetGUI()->SetControlFromPlug(cOutputVuMeter1, (mOutputVuMeterValue1 + 60) / 60);
		GetGUI()->SetControlFromPlug(cOutputVuMeter2, (mOutputVuMeterValue2 + 60) / 60);
	}

}
// End Processing per Framesize ans Process Double Replacing

/*##########################################################
						Hannes: RESET
##########################################################*/

void SRChannel::Reset()
{
	TRACE;
	IMutexLock lock(this);

	mSampleRate = GetSampleRate();
	InitBiquad();
	InitSafePan();
	InitCompPeak();
	InitCompRms();
	InitLimiter();
	InitDeesser();
	InitMeter();
	InitSaturation();
	circularBufferPointer = 0;
}

/*##########################################################
				Hannes: PARAMETER CHANGE
##########################################################*/

void SRChannel::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  mSampleRate = GetSampleRate();

  switch (paramIdx)
  {
	  
	  // INPUT AND OUTPUT STAGE

  case kInputGain: mInputGain = DBToAmp(GetParam(paramIdx)->Value()); break;
  //case kInputDrive: mInputDrive = DBToAmp(GetParam(paramIdx)->Value()); break;
  case kOutputGain: mOutputGain = DBToAmp(GetParam(paramIdx)->Value()); break;

  case kInputDrive: 
	  mInputDrive = GetParam(paramIdx)->Value(); 
	  fInputSaturation.setDrive(mInputDrive);
	  break;
  case kSaturationAmount:
	  mSaturationAmount = GetParam(paramIdx)->Value() / 100.;
	  fInputSaturation.setAmount(mSaturationAmount);
	  //mSatLfGain = (1 - mSaturationAmount) * 3.;		 fSatLfFilterL.setPeakGain(mSatLfGain);	  fSatLfFilterR.setPeakGain(mSatLfGain);
	  //mSatMfGain = (1 - mSaturationAmount) * -3.; 	 fSatMfFilterL.setPeakGain(mSatMfGain);	  fSatMfFilterR.setPeakGain(mSatMfGain);
	  //mSatHfGain = (1 - mSaturationAmount) * 5.; 	 fSatHfFilterL.setPeakGain(mSatHfGain);	  fSatHfFilterR.setPeakGain(mSatHfGain);
	  //mSatHpFreq = (1 - mSaturationAmount) * 30.;	fSatHpFilterL.setFc(mSatHpFreq / mSampleRate); fSatHpFilterR.setFc(mSatHpFreq / mSampleRate);
	  //mSatLpFreq = 22000 - (1 - mSaturationAmount) * 10000.;	fSatLpFilterL.setFc(mSatLpFreq / mSampleRate); fSatLpFilterR.setFc(mSatLpFreq / mSampleRate);
	  break;
  case kSaturationHarmonics: 
	  mSaturationHarmonics = GetParam(paramIdx)->Value() / 100.;
	  fInputSaturation.setHarmonics(mSaturationHarmonics);
	  break;
  case kSaturationSkew:
	  mSaturationSkew = GetParam(paramIdx)->Value() * 0.05;
	  fInputSaturation.setSkew(mSaturationSkew);
	  break;
  case kClipperThreshold: mClipperThreshold = 1. - GetParam(paramIdx)->Value() / 100.; break;
  case kLimiterThresh:
	  mLimiterThresh = GetParam(paramIdx)->Value();
	  GetParam(paramIdx)->SetDisplayText(10, "Off");
	  fLimiter.setThresh(mLimiterThresh);
	  break;
  case kPan: mPan = (GetParam(paramIdx)->Value() + 100) / 200; break;
  case kPanFreq:
	  mSafePanFreq = GetParam(paramIdx)->Value();
	  fSafePanHpL.setFc(mSafePanFreq / mSampleRate);
	  fSafePanHpR.setFc(mSafePanFreq / mSampleRate);
	  fSafePanLpL.setFc(mSafePanFreq / mSampleRate);
	  fSafePanLpR.setFc(mSafePanFreq / mSampleRate);
	  break;

	  // FILTER

  case kEqHpFreq:
	  mEqHpFreq = GetParam(paramIdx)->Value();
	  if (mEqHpOrder == 1 || mEqHpOrder == 3) {
		  fEqHpFilterOnepoleL.setFc(mEqHpFreq / mSampleRate); fEqHpFilterOnepoleR.setFc(mEqHpFreq / mSampleRate);
	  }
	  if (mEqHpOrder >= 2) {
		  fEqHpFilter1L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter1R.setFc(mEqHpFreq / mSampleRate);
	  }
	  if (mEqHpOrder >= 4) {
		  fEqHpFilter2L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter2R.setFc(mEqHpFreq / mSampleRate);
	  }
	  if (mEqHpOrder >= 5) {
		  fEqHpFilter3L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter3R.setFc(mEqHpFreq / mSampleRate);
	  }
	  if (mEqHpOrder >= 6) {
		  fEqHpFilter4L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter4R.setFc(mEqHpFreq / mSampleRate);
	  }
	  if (mEqHpOrder >= 7) {
		  fEqHpFilter5L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter5R.setFc(mEqHpFreq / mSampleRate);
	  }
	  if (mEqHpOrder >= 8) {
		  fEqHpFilter6L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter6R.setFc(mEqHpFreq / mSampleRate);
	  }
	  if (mEqHpOrder >= 9) {
		  fEqHpFilter7L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter7R.setFc(mEqHpFreq / mSampleRate);
		  fEqHpFilter8L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter8R.setFc(mEqHpFreq / mSampleRate);
		  fEqHpFilter9L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter9R.setFc(mEqHpFreq / mSampleRate);
		  fEqHpFilter10L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter10R.setFc(mEqHpFreq / mSampleRate);
	  }
	  GetParam(paramIdx)->SetDisplayText(16, "Off");
	  break;

  case kEqHpOrder:
	  GetParam(paramIdx)->SetDisplayText(1, "6");
	  GetParam(paramIdx)->SetDisplayText(2, "12");
	  GetParam(paramIdx)->SetDisplayText(3, "18");
	  GetParam(paramIdx)->SetDisplayText(4, "24");
	  GetParam(paramIdx)->SetDisplayText(5, "36");
	  GetParam(paramIdx)->SetDisplayText(6, "48");
	  GetParam(paramIdx)->SetDisplayText(7, "60");
	  GetParam(paramIdx)->SetDisplayText(8, "72");
	  GetParam(paramIdx)->SetDisplayText(9, "120");

	  mEqHpOrder = int(GetParam(paramIdx)->Value());
	  switch (mEqHpOrder) {
	  case 1:									// 1st order, 6 dB/Oct
		  break;
	  case 2:
		  fEqHpFilter1L.setQ(mEqPassQ_O2_F1);	// 2nd order, 12 dB/Oct
		  fEqHpFilter1R.setQ(mEqPassQ_O2_F1);
		  break;
	  case 3:									// 3rd order, 18 dB/Oct
		  fEqHpFilter1L.setQ(mEqPassQ_O3_F1);
		  fEqHpFilter1R.setQ(mEqPassQ_O3_F1);
		  break;
	  case 4:									// 4th order, 24 dB/Oct
		  fEqHpFilter1L.setQ(mEqPassQ_O4_F1);
		  fEqHpFilter1R.setQ(mEqPassQ_O4_F1);
		  fEqHpFilter2L.setQ(mEqPassQ_O4_F2);
		  fEqHpFilter2R.setQ(mEqPassQ_O4_F2);
		  break;
	  case 5:									// 6th order, 36 dB/Oct
		  fEqHpFilter1L.setQ(mEqPassQ_O6_F1);
		  fEqHpFilter1R.setQ(mEqPassQ_O6_F1);
		  fEqHpFilter2L.setQ(mEqPassQ_O6_F2);
		  fEqHpFilter2R.setQ(mEqPassQ_O6_F2);
		  fEqHpFilter3L.setQ(mEqPassQ_O6_F3);
		  fEqHpFilter3R.setQ(mEqPassQ_O6_F3);
		  break;
	  case 6:									// 8th order, 48 dB/Oct
		  fEqHpFilter1L.setQ(mEqPassQ_O8_F1);
		  fEqHpFilter1R.setQ(mEqPassQ_O8_F1);
		  fEqHpFilter2L.setQ(mEqPassQ_O8_F2);
		  fEqHpFilter2R.setQ(mEqPassQ_O8_F2);
		  fEqHpFilter3L.setQ(mEqPassQ_O8_F3);
		  fEqHpFilter3R.setQ(mEqPassQ_O8_F3);
		  fEqHpFilter4L.setQ(mEqPassQ_O8_F4);
		  fEqHpFilter4R.setQ(mEqPassQ_O8_F4);
		  break;
	  case 7:									// 10th order, 48 dB/Oct
		  fEqHpFilter1L.setQ(mEqPassQ_O10_F1);
		  fEqHpFilter1R.setQ(mEqPassQ_O10_F1);
		  fEqHpFilter2L.setQ(mEqPassQ_O10_F2);
		  fEqHpFilter2R.setQ(mEqPassQ_O10_F2);
		  fEqHpFilter3L.setQ(mEqPassQ_O10_F3);
		  fEqHpFilter3R.setQ(mEqPassQ_O10_F3);
		  fEqHpFilter4L.setQ(mEqPassQ_O10_F4);
		  fEqHpFilter4R.setQ(mEqPassQ_O10_F4);
		  fEqHpFilter5L.setQ(mEqPassQ_O10_F5);
		  fEqHpFilter5R.setQ(mEqPassQ_O10_F5);
		  break;
	  case 8:									// 12th order, 72 db/Oct
		  fEqHpFilter1L.setQ(mEqPassQ_O12_F1);
		  fEqHpFilter1R.setQ(mEqPassQ_O12_F1);
		  fEqHpFilter2L.setQ(mEqPassQ_O12_F2);
		  fEqHpFilter2R.setQ(mEqPassQ_O12_F2);
		  fEqHpFilter3L.setQ(mEqPassQ_O12_F3);
		  fEqHpFilter3R.setQ(mEqPassQ_O12_F3);
		  fEqHpFilter4L.setQ(mEqPassQ_O12_F4);
		  fEqHpFilter4R.setQ(mEqPassQ_O12_F4);
		  fEqHpFilter5L.setQ(mEqPassQ_O12_F5);
		  fEqHpFilter5R.setQ(mEqPassQ_O12_F5);
		  fEqHpFilter6L.setQ(mEqPassQ_O12_F6);
		  fEqHpFilter6R.setQ(mEqPassQ_O12_F6);
		  break;
	  case 9:									// 20th order, 120 dB/Oct
		  fEqHpFilter1L.setQ(mEqPassQ_O20_F1);
		  fEqHpFilter1R.setQ(mEqPassQ_O20_F1);
		  fEqHpFilter2L.setQ(mEqPassQ_O20_F2);
		  fEqHpFilter2R.setQ(mEqPassQ_O20_F2);
		  fEqHpFilter3L.setQ(mEqPassQ_O20_F3);
		  fEqHpFilter3R.setQ(mEqPassQ_O20_F3);
		  fEqHpFilter4L.setQ(mEqPassQ_O20_F4);
		  fEqHpFilter4R.setQ(mEqPassQ_O20_F4);
		  fEqHpFilter5L.setQ(mEqPassQ_O20_F5);
		  fEqHpFilter5R.setQ(mEqPassQ_O20_F5);
		  fEqHpFilter6L.setQ(mEqPassQ_O20_F6);
		  fEqHpFilter6R.setQ(mEqPassQ_O20_F6);
		  fEqHpFilter7L.setQ(mEqPassQ_O20_F7);
		  fEqHpFilter7R.setQ(mEqPassQ_O20_F7);
		  fEqHpFilter8L.setQ(mEqPassQ_O20_F8);
		  fEqHpFilter8R.setQ(mEqPassQ_O20_F8);
		  fEqHpFilter9L.setQ(mEqPassQ_O20_F9);
		  fEqHpFilter9R.setQ(mEqPassQ_O20_F9);
		  fEqHpFilter10L.setQ(mEqPassQ_O20_F10);
		  fEqHpFilter10R.setQ(mEqPassQ_O20_F10);
		  break;
	  default:
		  break;
	  }
	  OnParamChange(kEqHpFreq);
	  break;

  case kEqLpOrder:
	  GetParam(paramIdx)->SetDisplayText(1, "6");
	  GetParam(paramIdx)->SetDisplayText(2, "12");
	  GetParam(paramIdx)->SetDisplayText(3, "18");
	  GetParam(paramIdx)->SetDisplayText(4, "24");
	  GetParam(paramIdx)->SetDisplayText(5, "36");
	  GetParam(paramIdx)->SetDisplayText(6, "48");
	  GetParam(paramIdx)->SetDisplayText(7, "60");
	  GetParam(paramIdx)->SetDisplayText(8, "72");
	  GetParam(paramIdx)->SetDisplayText(9, "120");
	  break;

  case kEqLpFreq:
	  mEqLpFreq = GetParam(paramIdx)->Value();
	  fEqLpFilter1L.setFc(mEqLpFreq / mSampleRate);
	  fEqLpFilter1R.setFc(mEqLpFreq / mSampleRate);
	  GetParam(paramIdx)->SetDisplayText(22000, "Off");
	  break;

	  // EQUALIZER

  case kEqHfBell:
	  mEqHfIsBell = GetParam(paramIdx)->Value();
	  if (mEqHfIsBell == 1) {fEqHfFilterL.setType(SRPlugins::SRFilters::biquad_peak); fEqHfFilterR.setType(SRPlugins::SRFilters::biquad_peak);}
	  else {fEqHfFilterL.setType(SRPlugins::SRFilters::biquad_highshelf); fEqHfFilterR.setType(SRPlugins::SRFilters::biquad_highshelf);}
	  break;

  case kEqLfBell:
	  mEqLfIsBell = GetParam(paramIdx)->Value();
	  if (mEqLfIsBell == 1) {fEqLfFilterL.setType(SRPlugins::SRFilters::biquad_peak); fEqLfFilterR.setType(SRPlugins::SRFilters::biquad_peak);}
	  else {fEqLfFilterL.setType(SRPlugins::SRFilters::biquad_lowshelf); fEqLfFilterR.setType(SRPlugins::SRFilters::biquad_lowshelf);}
	  break;

  case kEqLfGain: mEqLfGain = GetParam(paramIdx)->Value(); fEqLfFilterL.setPeakGain(mEqLfGain); fEqLfFilterR.setPeakGain(mEqLfGain); break;
  case kEqLfFreq: mEqLfFreq = GetParam(paramIdx)->Value(); fEqLfFilterL.setFc(mEqLfFreq / mSampleRate); fEqLfFilterR.setFc(mEqLfFreq / mSampleRate); break;
  case kEqLfQ: mEqLfQ = GetParam(paramIdx)->Value(); fEqLfFilterL.setQ(mEqLfQ); fEqLfFilterR.setQ(mEqLfQ); break;

  case kEqLmfGain: mEqLmfGain = GetParam(paramIdx)->Value(); fEqLmfFilterL.setPeakGain(mEqLmfGain); fEqLmfFilterR.setPeakGain(mEqLmfGain); break;
  case kEqLmfFreq: mEqLmfFreq = GetParam(paramIdx)->Value(); fEqLmfFilterL.setFc(mEqLmfFreq / mSampleRate); fEqLmfFilterR.setFc(mEqLmfFreq / mSampleRate); break;
  case kEqLmfQ: mEqLmfQ = GetParam(paramIdx)->Value(); fEqLmfFilterL.setQ(mEqLmfQ); fEqLmfFilterR.setQ(mEqLmfQ); break;

  case kEqHmfGain: mEqHmfGain = GetParam(paramIdx)->Value(); fEqHmfFilterL.setPeakGain(mEqHmfGain); fEqHmfFilterR.setPeakGain(mEqHmfGain); break;
  case kEqHmfFreq: mEqHmfFreq = GetParam(paramIdx)->Value(); fEqHmfFilterL.setFc(mEqHmfFreq / mSampleRate); fEqHmfFilterR.setFc(mEqHmfFreq / mSampleRate); break;
  case kEqHmfQ: mEqHmfQ = GetParam(paramIdx)->Value(); fEqHmfFilterL.setQ(mEqHmfQ); fEqHmfFilterR.setQ(mEqHmfQ); break;

  case kEqHfGain: mEqHfGain = GetParam(paramIdx)->Value(); fEqHfFilterL.setPeakGain(mEqHfGain); fEqHfFilterR.setPeakGain(mEqHfGain); break; 
  case kEqHfFreq: mEqHfFreq = GetParam(paramIdx)->Value(); fEqHfFilterL.setFc(mEqHfFreq / mSampleRate); fEqHfFilterR.setFc(mEqHfFreq / mSampleRate); break;
  case kEqHfQ: mEqHfQ = GetParam(paramIdx)->Value(); fEqHfFilterL.setQ(mEqHfQ); fEqHfFilterR.setQ(mEqHfQ); break;

	  // COMPRESSOR

  case kCompIsParallel: 
	  mCompIsParallel = GetParam(paramIdx)->Value();
	  if (GetGUI()) {
		  //(mCompIsParallel == 0) ? GetGUI()->GrayOutControl(kCompPeakRmsRatio, true) : GetGUI()->GrayOutControl(kCompPeakRmsRatio, false);
		  GrayOutControls();
	  }
	  break;

  case kCompPeakIsExtSc: mCompPeakIsExtSc = GetParam(paramIdx)->Value(); break;
  case kCompRmsIsExrSc: mCompRmsIsExtSc = GetParam(paramIdx)->Value(); break;

  case kCompPeakRatio:
	  mCompPeakRatio = (1. / GetParam(paramIdx)->Value());
	  if (mCompPeakRatio <= 1./20.) {
		  GetParam(paramIdx)->SetDisplayText(20, "inf");
		  mCompPeakRatio = 0.;
	  }
	  fCompressorPeak.setRatio(mCompPeakRatio);
	  mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease);
	  break;

	  // Thresh in dB !!
  case kCompPeakThresh:
	  mCompPeakThresh = GetParam(paramIdx)->Value();
	  fCompressorPeak.setThresh(mCompPeakThresh);
	  mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease);
	  break;

	  // Attack in ms
  case kCompPeakAttack:
	  mCompPeakAttack = GetParam(paramIdx)->Value();
	  fCompressorPeak.setAttack(mCompPeakAttack);
	  mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease);
	  break;

	  // Release in ms
  case kCompPeakRelease:
	  mCompPeakRelease = GetParam(paramIdx)->Value();
	  fCompressorPeak.setRelease(mCompPeakRelease);
	  mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease);
	  break;

  case kCompPeakKneeWidthDb:
	  mCompPeakKneeWidthDb = GetParam(paramIdx)->Value();
	  fCompressorPeak.setKnee(mCompPeakKneeWidthDb);
	  break;

  case kCompPeakSidechainFilterFreq:
	  mCompPeakSidechainFilterFreq = GetParam(paramIdx)->Value();
	  fCompressorPeak.setSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate);
	  GetParam(paramIdx)->SetDisplayText(16, "Off");
	  break;

  case kCompRmsRatio:
	  mCompRmsRatio = (1 / GetParam(paramIdx)->Value());
	  fCompressorRms.setRatio(mCompRmsRatio);
	  // Auto Makeup
	  mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease);
	  break;

	  // Thresh in dB !!
  case kCompRmsThresh:
	  mCompRmsThresh = GetParam(paramIdx)->Value();
	  fCompressorRms.setThresh(mCompRmsThresh);
	  // Auto Makeup
	  mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease);
	  break;

	  // Attack in ms
  case kCompRmsAttack:
	  mCompRmsAttack = GetParam(paramIdx)->Value();
	  fCompressorRms.setAttack(mCompRmsAttack);
	  mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease);
	  break;

	  // Release in ms
  case kCompRmsRelease:
	  mCompRmsRelease = GetParam(paramIdx)->Value();
	  fCompressorRms.setRelease(mCompRmsRelease);
	  mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease);
	  break;

  case kCompRmsKneeWidthDb:
	  mCompRmsKneeWidthDb = GetParam(paramIdx)->Value();
	  fCompressorRms.setKnee(mCompRmsKneeWidthDb);
	  break;

  case kCompPeakRmsRatio:
	  mCompPeakRmsRatio = GetParam(paramIdx)->Value() / 100;
	  break;

	  // RMS Makeup (db -> amp)
  case kCompRmsMakeup: mCompRmsMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;
  case kCompPeakMakeup: mCompPeakMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;
  case kCompDryWet: mCompDryWet = GetParam(paramIdx)->Value() / 100; break;
	  
	  // GLOBAL BYPASS
  case kEqBypass: mEqBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
  case kCompBypass: mCompBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
  case kInputBypass: mInputBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
  case kOutputBypass: mOutputBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
  case kBypass: mBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;

	  // Automatic Gain Control

  case kAgc:
	  if (circularBufferInL[circularBufferLenght-1]) {
		  sumIn = 0.;
		  sumOut = 0.;
		  aveIn = 0.;
		  aveOut = 0.;
		  diffInOut = 0.;
		  for (int i = 0; i <= circularBufferLenght-1; i++) {
			  sumIn += fabs(circularBufferInL[i]) + fabs(circularBufferInR[i]);
			  sumOut += fabs(circularBufferOutL[i]) + fabs(circularBufferOutR[i]);
		  }
		  aveIn = sumIn / (2. * (double(circularBufferLenght)));
		  aveOut = sumOut / (2. * (double(circularBufferLenght)));
		  diffInOut = sumIn / sumOut;
		  if (GetGUI()) {
			  if (diffInOut > 4.) {
				  GetGUI()->SetParameterFromGUI(kOutputGain, ToNormalizedParam(AmpToDB(4.), GetParam(kOutputGain)->GetMin(), GetParam(kOutputGain)->GetMax(), GetParam(kOutputGain)->GetShape()));
			  }
			  else if (diffInOut < .125) {
				  GetGUI()->SetParameterFromGUI(kOutputGain, ToNormalizedParam(AmpToDB(.125), GetParam(kOutputGain)->GetMin(), GetParam(kOutputGain)->GetMax(), GetParam(kOutputGain)->GetShape()));
			  }
			  else {
				  GetGUI()->SetParameterFromGUI(kOutputGain, ToNormalizedParam(AmpToDB(diffInOut), GetParam(kOutputGain)->GetMin(), GetParam(kOutputGain)->GetMax(), GetParam(kOutputGain)->GetShape()));
			  }
		  }
	  }
	  break;

	  // Deesser

  case kDeesserBottomFreq:
	  mDeesserBottomFreq = GetParam(paramIdx)->Value();
	  fDeesserBottomLpFilterL.setFc(mDeesserBottomFreq / mSampleRate);
	  fDeesserBottomLpFilterR.setFc(mDeesserBottomFreq / mSampleRate);
	  fDeesserMidHpFilterL.setFc(mDeesserBottomFreq / mSampleRate);
	  fDeesserMidHpFilterR.setFc(mDeesserBottomFreq / mSampleRate);
	  break;

  case kDeesserTopFreq:
	  mDeesserTopFreq = GetParam(paramIdx)->Value();
	  fDeesserMidLpFilterL.setFc(mDeesserTopFreq / mSampleRate);
	  fDeesserMidLpFilterR.setFc(mDeesserTopFreq / mSampleRate);
	  fDeesserTopHpFilterL.setFc(mDeesserTopFreq / mSampleRate);
	  fDeesserTopHpFilterR.setFc(mDeesserTopFreq / mSampleRate);
	  break;

  case kDeesserThresh: mDeesserThresh = GetParam(paramIdx)->Value(); fDeesser.setThresh(mDeesserThresh); break;
  case kDeesserRatio: mDeesserRatio = (1. / GetParam(paramIdx)->Value()); fDeesser.setRatio(mDeesserRatio); break;
  case kDeesserAttack: mDeesserAttack = GetParam(paramIdx)->Value(); fDeesser.setAttack(mDeesserAttack); break;
  case kDeesserRelease: mDeesserRelease= GetParam(paramIdx)->Value(); fDeesser.setRelease(mDeesserRelease); break;
  case kDeesserMakeup: mDeesserMakeup= DBToAmp(GetParam(paramIdx)->Value()); break;
	  
	  // TEST PARAMS

  case kTestParam1: mTestParam1 = GetParam(paramIdx)->Value(); break;
  case kTestParam2: mTestParam2 = GetParam(paramIdx)->Value(); break;
  case kTestParam3: mTestParam3 = GetParam(paramIdx)->Value(); break;
  case kTestParam4: mTestParam4 = GetParam(paramIdx)->Value(); break;
  case kTestParam5: mTestParam5 = GetParam(paramIdx)->Value(); break;

  default: break;
  }

}