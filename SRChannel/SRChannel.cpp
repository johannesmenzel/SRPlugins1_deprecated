#include "SRChannel.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "denormal.h"

const int kNumPrograms = 1;									// Here you find the total number of presets defined
const double METER_ATTACK = .6, METER_DECAY = .05;			// This is the global attack and release constants for meters
const double halfpi = PI / 2;								// Half pi defined
const double dcoff = 1e-15;									// amount of DC offset added and subtracted respectivly at the beginning and the end of DoubleReplacing loop

const double mEqPassQ_O2_F1 = 0.70710678;					// Setting the Q constants f�r higher order low- and highpass filter.
const double stQ = mEqPassQ_O2_F1;
const double mEqPassQ_O3_F1 = 1.;
const double mEqPassQ_O4_F1 = 0.54119610;
const double mEqPassQ_O4_F2 = 1.3065630;
const double mEqPassQ_O6_F1 = 0.51763809;
const double mEqPassQ_O6_F2 = 0.70710678;
const double mEqPassQ_O6_F3 = 1.9318517;
const double mEqPassQ_O8_F1 = 0.50979558;
const double mEqPassQ_O8_F2 = 0.60134489;
const double mEqPassQ_O8_F3 = 0.89997622;
const double mEqPassQ_O8_F4 = 2.5629154;
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
	kScaleX = 50,											// Horizontal spacing of controls
	kScaleY = 38,
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
	kDeesserFreq,
	kDeesserQ,
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
	kIsPanMonoLow,
	kSaturationType,
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
	none,
	kNumKnobs
};

// All parameters possible data types listed here
enum Type {
	typeDouble = 1,
	typeBool,
	typeInt,
	typeEnum,
	kNumDataType
};

enum Order {
	dbo6 = 0,
	dbo12,
	dbo18,
	dbo24,
	dbo36,
	dbo48,
	dbo60,
	dbo72,
	dbo120,
	kNumOrders
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
	//	{ "NAME",			"SNAME"	DEF,	MIN,	MAX,	STEP,	CENTER,	CTRPNT,	"LABEL",GROUP ,		TYPE,		KNOB,		kControlX + kScaleX * X,		kControlY + kScaleY * Y,		"LBLMIN", "LBLMAX", "LBLCTR", "TOOLTIP"},
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
		{ "Sat Amount",		"AMT",	0.,		0.,		99.,	0.01,	10.,	.5,		"%",	"Input",	typeDouble, SslOrange,	kControlX + kScaleX * 2,		kControlY + kScaleY * 0,		"0", "100", "50",		"Amount of Saturation" },
		{ "Clipper",		"CLP",	0.,		0.,		99.,	0.01,	50.,	.5,		"%",	"Output",	typeDouble, SslYellow,	kControlX + kScaleX * 16,		kControlY + kScaleY * 0,		"0", "100", "50",		"Amount of the Output Clipper (cuts amplitutde)" },
		{ "Output Gain",	"OUT",	0.,		-60.,	12.,	0.1,	0.,		10. / 12.,"dB",	"Global",	typeDouble, Fader,		kControlX + kScaleX * 16,		kControlY + kScaleY * 2,		"-60", "12", "0",		"Output Gain" },
		{ "Pan",			"PAN",	0.,		-100.,	100.,	1.,		0.,		.5,		"%",	"Output",	typeDouble, SslBlue,	kControlX + kScaleX * 14,		kControlY + kScaleY * 1,		"L", "R", "C",			"Pan" },
		{ "Pan Freq",		"PNF",	150.,	20.,	1000.,	1.,		150.,	.5,		"Hz",	"Output",	typeDouble, SslRed,		kControlX + kScaleX * 14,		kControlY + kScaleY * 3,		"20", "1k", "200",		"Frequencies below crossover will be not affected by panner" },
		{ "Limiter",		"LMT",	10.,	-30.,	10.,	0.1,	0.,		.5,		"dB",	"Output",	typeDouble, SslOrange,	kControlX + kScaleX * 14,		kControlY + kScaleY * 5,		"-30", "10", "0",		"Thresold of Output Limiter" },
		{ "Comp Ser/Par",	"Par",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,		kControlX + kScaleX * 9,		kControlY + kScaleY * 16,		"SER", "PAR", "",		"RMS and Peak Compressor can be run serial or parallel" },
		{ "EQ Bypass",		"EQ Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,		kControlX + kScaleX * 7 - 32,	kControlY + kScaleY * 0 - 36,	"ACT", "BYP", "",		"Bypass Equalizer Section" },
		{ "Comp Bypass",	"Comp Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,		kControlX + kScaleX * 13 - 32,	kControlY + kScaleY * 0 - 36,	"ACT", "BYP", "",		"Bypass Compressor Section" },
		{ "Output Bypass",	"Out Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,		kControlX + kScaleX * 17 - 32,	kControlY + kScaleY * 0 - 36,	"ACT", "BYP", "",		"Bypass Output Section" },
		{ "Bypass",			"Byp",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,		4,								4,								"ACT", "BYP", "",		"Bypass Plugin" },
		{ "Harmonics",		"HRM",	50.,	0.,		100.,	.01,	50.,	.5,		"%",	"Input",	typeDouble, SslBlue,	kControlX + kScaleX * 2,		kControlY + kScaleY * 2,		"Even", "Odd", "Mix",	"Dial in even harmonics by turning the knob counter-clockwise" },
		{ "HP Order",		"HPO",	1,		0,		8,		1,		5,		.5,		"dB/oct", "EQ",		typeEnum,	none,		kControlX + kScaleX * 4,		kControlY + kScaleY * 0,		"6", "120", "36",		"Order of the Highpass Filter or filter slope" },
		{ "LP Order",		"LPO",	1,		0,		8,		1,		5,		.5,		"dB/oct", "EQ",		typeEnum,	none,		kControlX + kScaleX * 6,		kControlY + kScaleY * 2,		"6", "120", "36",		"HIDED - Order of the Lowpass Filter or filter slope" },
		{ "TestParam 1",	"T1",	0.2,	0.2,	5.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 4,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
		{ "TestParam 2",	"T2",	0.2,	0.2,	5.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 6,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
		{ "TestParam 3",	"T3",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 8,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
		{ "TestParam 4",	"T4",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 10,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
		{ "TestParam 5",	"T5",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		kControlX + kScaleX * 12,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
		{ "Sat Drive",		"DRV",	0.,		0.,		60.,	0.1,	30.,	.5,		"dB",	"Input",	typeDouble, SslRed,		kControlX + kScaleX * 0,		kControlY + kScaleY * 0,		"0", "60", "30",		"Saturation input drive, hits the saturation module harder" },
		{ "Auto Gain Comp",	"AGC",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Output",	typeBool,	Button,		kControlX + kScaleX * 14,		kControlY + kScaleY * 9,		"AGC", "", "",			"Automatic Gain Conpensation. Click to adjust output volume to input volume" },
		{ "Peak SC Filter",	"SCF",	16.,	16.,	5000.,	1.,		1000.,	.5,		"Hz",	"Compressor",typeDouble, SslBlue,	kControlX + kScaleX * 8,		kControlY + kScaleY * 14,		"16", "5k", "1k",		"Frequency of the Compressors sidechain highpass filter" },
		{ "Deesser Freq",	"DSF",	7000.,	16.,	22000.,	1.,		7000.,	.5,		"Hz",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 0,		"16", "22k", "7k",		"Deessers center frequency" },
		{ "Deesser Q",		"DSQ",	stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 2,		"W", "N", "",			"Deessers width (Q)" },
		{ "Deesser Thresh",	"DST",	0.,		-100.,	0.,		0.1,	-50.,	.5,		"dB",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 4,		"-100", "0", "-50",		"Deessers Threshold" },
		{ "Deesser Ratio",	"DSR",	3.,		0.5,	20.,	0.01,	3.,		.5,		":1",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 6,		".5:1", "20:1", "3:1",	"Deessers Ratio" },
		{ "Deesser Attack",	"DSA",	20.,	0.01,	200.,	0.01,	20.,	.5,		"ms",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 8,		"0.01", "200", "20",	"Deessers Attack" },
		{ "Deesser Release","DSD",	200.,	1.,		2000.,	0.01,	200.,	.5,		"ms",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 10,		"1", "2k", "200",		"Deessers Release" },
		{ "Deesser Makeup",	"DSM",	0.,		0.,		40.,	0.1,	10.,	.5,		"dB",	"Deesser",	typeDouble, SslYellow,	kControlX + kScaleX * 12,		kControlY + kScaleY * 12,		"0", "40", "10",		"Deessers Makeup" },
		{ "Peak Knee",		"SKN",	0.,		0.,		30.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslGreen,	kControlX + kScaleX * 10,		kControlY + kScaleY * 13,		"0", "30", "10",		"Peak Compressors width of the soft knee" },
		{ "RMS Knee",		"SKN",	0.,		0.,		30.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslGreen,	kControlX + kScaleX * 10,		kControlY + kScaleY * 5,		"0", "30", "10",		"RMS Compressors width of the soft knee" },
		{ "Input Bypass",	"In Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,		kControlX + kScaleX * 3 - 32,	kControlY + kScaleY * 0 - 36,	"ACT", "BYP", "",		"Bypass Input Section" },
		{ "Peak Ext SC",	"Ext",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,		kControlX + kScaleX * 10,		kControlY + kScaleY * 15,		"INT", "EXT", "",		"External sidechain for Peak Compressor. Use tracks input channel 3/4" },
		{ "RMS Ext SC",		"Ext",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,		kControlX + kScaleX * 8,		kControlY + kScaleY * 6,		"INT", "EXT", "",		"External sidechain for RMS Compressor. Use tracks input channel 3/4" },
		{ "Sat Skew",		"SKW",	0,		-100.,	100.,	0.01,	0.,		.5,		"%",	"Input",	typeDouble, SslOrange,	kControlX + kScaleX * 2,		kControlY + kScaleY * 4,		"-100", "100", "0",		"Saturations positive/negative skewness. Distorts waveform" },
		{ "Mono Bass",		"MNB",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Output",	typeBool,	Button,		kControlX + kScaleX * 14,		kControlY + kScaleY * 7,		"STB", "MNB", "",		"Bass Frequencies will left stereo or will be monoed" },
		{ "Saturation Type","TPE",	0,		0,		3,		-1,		-1,		-1,		"",		"Input",	typeEnum,	none,			kControlX + kScaleX * 2,		kControlY + kScaleY * 6,		"", "", "",				"Type of Saturation"}
		//	{ "NAME",		"SNAME"	DEF,	MIN,	MAX,	STEP,	CENTER,	CTRPNT,	"LABEL",GROUP ,		TYPE,		KNOB,		kControlX + kScaleX * X,		kControlY + kScaleY * Y,		"LBLMIN", "LBLMAX", "LBLCTR", "TOOLTIP"},
};


/*##########################################################
						CONSTRUCTOR
##########################################################*/

SRChannel::SRChannel(IPlugInstanceInfo instanceInfo)
	: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
	mInputPeakMeterValue1(0.),
	mInputPeakMeterValue2(0.),
	mRmsGrMeterValue(0.),
	mPeakGrMeterValue(0.),
	mDeesserMeterValue(0.),
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

		switch (properties.Type) {							// each type of parameter needs another initialization method

		case typeBool:									// Create parameters of type boolean.
			param->InitBool(
				properties.name,
				bool(properties.defaultVal),
				properties.label,
				properties.group
			);
			break;

		case typeEnum:									// When initializing enum parameter, for VST3 this is necessary: param->SetDisplayText(0, properties.name);

			int enumLenght;								// Get number of enum states
			switch (i) {
			case kEqHpOrder: case kEqLpOrder: enumLenght = kNumOrders; break;
			case kSaturationType: enumLenght = SRPlugins::SRSaturation::numTypes; break;
			default: break;
			}

			// Create Enum Parameter
			param->InitEnum(properties.name, int(properties.defaultVal), enumLenght, properties.label, properties.group);

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
				SRPlugins::SRHelpers::SetShapeCentered(properties.minVal, properties.maxVal, properties.centerVal, properties.centerPoint) // We calculate the controls shape.
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

		default:											// Just in case there are paramters of other types.
			break;
		}


		// Setup display texts
		switch (i)
		{
		case kEqHpOrder:
		case kEqLpOrder:
			GetParam(i)->SetDisplayText(dbo6, "6 dB/oct");
			GetParam(i)->SetDisplayText(dbo12, "12 dB/oct");
			GetParam(i)->SetDisplayText(dbo18, "18 dB/oct");
			GetParam(i)->SetDisplayText(dbo24, "24 dB/oct");
			GetParam(i)->SetDisplayText(dbo36, "36 dB/oct");
			GetParam(i)->SetDisplayText(dbo48, "48 dB/oct");
			GetParam(i)->SetDisplayText(dbo60, "60 dB/oct");
			GetParam(i)->SetDisplayText(dbo72, "72 dB/oct");
			GetParam(i)->SetDisplayText(dbo120, "120 dB/oct");
			break;
		case kSaturationType:
			GetParam(i)->SetDisplayText(SRPlugins::SRSaturation::typeMusicDSP, "MusicDSP");
			GetParam(i)->SetDisplayText(SRPlugins::SRSaturation::typeZoelzer, "Zoelzer");
			GetParam(i)->SetDisplayText(SRPlugins::SRSaturation::typePirkle, "Pirkle");
			GetParam(i)->SetDisplayText(SRPlugins::SRSaturation::typePirkleModified, "Pirkle Mod");
			break;
		case kLimiterThresh: GetParam(i)->SetDisplayText(10, "Off"); break;
		case kEqHpFreq: case kCompPeakSidechainFilterFreq: GetParam(i)->SetDisplayText(16, "Off"); break;
		case kEqLpFreq: GetParam(i)->SetDisplayText(22000, "Off"); break;
		case kCompPeakRatio: GetParam(i)->SetDisplayText(20, "inf"); break;
		default: break;
		}

		OnParamChange(i);									// Run OnParamChange() once for each parameter to initialize member variables and so on.

	}
}


void SRChannel::CreatePresets() {
	// These have to be created when Parameters are complete, so work in progress.
	// Following are original procedures to make default presets
		// MakeDefaultPreset((char *) "Full Reset", kNumPrograms);
		// MakePreset("Default", kNumPrograms);
	// Here's the beginning of the real stuff:
	MakeDefaultPreset("Default", 1);
	// It may work like this:
//MakePresetFromNamedParams("Kick", kInputGain, 0.0, kEqHpFreq, kEqLpFreq, kEqHfGain, kEqHfFreq, kEqHfQ, kEqHfBell, kEqHmfGain, kEqHmfFreq, kEqHmfQ, kEqLmfGain, kEqLmfFreq, kEqLmfQ, kEqLfGain, kEqLfFreq, kEqLfQ, kEqLfBell, kCompRmsThresh, kCompRmsRatio, kCompRmsAttack, kCompRmsRelease, kCompRmsMakeup, kCompPeakThresh, kCompPeakRatio, kCompPeakAttack, kCompPeakRelease, kCompPeakMakeup, kCompPeakRmsRatio, kCompDryWet, kSaturationAmount, kClipperThreshold, kOutputGain, kPan, kPanFreq, kLimiterThresh, kCompIsParallel, kEqBypass, kCompBypass, kOutputBypass, kBypass, kSaturationHarmonics, kEqHpOrder, kEqLpOrder, kTestParam1, kTestParam2, kTestParam3, kTestParam4, kTestParam5, kInputDrive, kAgc, kCompPeakSidechainFilterFreq, kDeesserBottomFreq, kDeesserTopFreq, kDeesserThresh, kDeesserRatio, kDeesserAttack, kDeesserRelease, kDeesserMakeup, kCompPeakKneeWidthDb, kCompRmsKneeWidthDb, kInputBypass, kCompPeakIsExtSc, kCompRmsIsExrSc, kSaturationSkew);
}


// Method to gray controls, which are currently bypassed. Thats why you have to test from top to bottom
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
					|| i == kEqLpOrder) ? grayout = true
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
	IColor colorMeterFg = IColor(/*1*/ 50, 255, 120, 0);
	IColor colorMeterBg = IColor(/*50*/0, 0, 0, 0);

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
	pGraphics->AttachControl(new ITextControl(this, IRECT(kWidth - 300, 0, kWidth, 20), &textPresetLabelProp, "v" VST3_VER_STR));

	// Preset Menu
	pGraphics->AttachControl(new SRPlugins::SRControls::IPresetMenu(this, IRECT(402, 4, 698, 24), &textPresetLabelProp));

	// Meters
		// Peak and GR
	IRECT meter = IRECT(kWidth - 60, kControlY, kWidth, kHeight);
	cInputPeakMeter1 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 59, bitmapLogo.H, kWidth - 51, kHeight), colorMeterBg, colorMeterFg));
	cInputPeakMeter2 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 49, bitmapLogo.H, kWidth - 41, kHeight), colorMeterBg, colorMeterFg));
	cRmsGrMeter = pGraphics->AttachControl(new SRPlugins::SRControls::IGrMeterVert(this, IRECT(kWidth - 39, bitmapLogo.H, kWidth - 31, kHeight), colorMeterBg, colorMeterFg));
	cPeakGrMeter = pGraphics->AttachControl(new SRPlugins::SRControls::IGrMeterVert(this, IRECT(kWidth - 29, bitmapLogo.H, kWidth - 21, kHeight), colorMeterBg, colorMeterFg));
	cDeesserMeter = pGraphics->AttachControl(new SRPlugins::SRControls::IGrMeterVert(this, IRECT(kWidth - 29, bitmapLogo.H, kWidth - 21, kHeight), colorMeterBg, colorMeterFg));
	cOuputPeakMeter1 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 19, bitmapLogo.H, kWidth - 11, kHeight), colorMeterBg, colorMeterFg));
	cOutputPeakMeter2 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 9, bitmapLogo.H, kWidth - 1, kHeight), colorMeterBg, colorMeterFg));
	cOutputVuMeter1 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 19, bitmapLogo.H, kWidth - 11, kHeight), colorMeterBg, colorMeterFg));
	cOutputVuMeter2 = pGraphics->AttachControl(new SRPlugins::SRControls::IPeakMeterVert(this, IRECT(kWidth - 9, bitmapLogo.H, kWidth - 1, kHeight), colorMeterBg, colorMeterFg));


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
		case none: knob = 0;
		}

		IRECT knobspace = IRECT(properties.x, properties.y, properties.x + 2 * kScaleX, properties.y + 2 * kScaleY);

		int knobwidth = (knob != 0) ? knob->frameWidth() : 0;
		int knobheight = (knob != 0) ? knob->frameHeight() : 0;

		int numFaderLabels = 12;


		switch (properties.Type) {						// Testing for type of parameter, bools get buttons, doubles get knobs a.s.o.a.s.f.




		// Boolean Parameters
		case typeBool:

			// SWITCHES
			// ------------------------------------------------------------------

			// Every bool switch gets labels
			pGraphics->AttachControl(new ITextControl(this, IRECT(
				properties.x + 32,
				properties.y + 0,
				properties.x + 50,
				properties.y + 16
			), &textKnobRingMaxProp, properties.labelMax));
			pGraphics->AttachControl(new ITextControl(this, IRECT(
				properties.x + 32,
				properties.y + 16,
				properties.x + 50,
				properties.y + 32
			), &textKnobRingMaxProp, properties.labelMin));

			switch (i)
			{
			case kAgc:
				cControlMatrix.at(i) = pGraphics->AttachControl(
					new IContactControl(this, properties.x, properties.y, i, knob)
				); break;
			default:
				cControlMatrix.at(i) = pGraphics->AttachControl(
					new ISwitchControl(this, properties.x, properties.y, i, knob)
				); break;
			}

			// ------------------------------------------------------------------
			// SWITCHES

			break; // End bool




		case typeEnum:
			//pGraphics->AttachControl(new ISwitchPopUpControl(this, properties.x, properties.y, i, knob));
			cControlMatrix.at(i) = pGraphics->AttachControl(new SRPlugins::SRControls::IPopUpMenuControl(this, IRECT(
				properties.x,
				properties.y,
				properties.x + 60,
				properties.y + textFg.mSize
			), i, &textFg, colorBg));
			break; // End enum





				   // Double AND int Paramters
		case typeDouble:
		case typeInt:
			switch (i)
			{

				// Faders
			case kInputGain:
			case kOutputGain:

				// FADER
				// ------------------------------------------------------------------

				// Faders volume labels
				for (int j = 0; j <= numFaderLabels; j++) {
					float scaleLabels = 1.f / numFaderLabels;
					std::string faderLabelString = std::to_string((-j + 2) * 6);
					char const *faderLabelChar = faderLabelString.c_str();
					pGraphics->AttachControl(new ITextControl(this, IRECT(
						properties.x + 48,
						int(properties.y + knob->H * .5
							+ pow(j * scaleLabels, GetParam(i)->GetShape())
							* (kHeight - properties.y - 14 - knob->H) - 7),
						properties.x + 64,
						int(properties.y + knob->H * .5
							+ pow(j * scaleLabels, GetParam(i)->GetShape())
							* (kHeight - properties.y - 14 - knob->H) + 7)
					), &textKnobRingCtrProp, faderLabelChar));
				}

				// Fader Background
				pGraphics->AttachControl(new IPanelControl(this, IRECT((int(
					properties.x + knob->W * .5) - 4),
					(properties.y),
					(int(properties.x + knob->W * .5) + 4),
					kHeight - 10
				), &COLOR_BLACK));

				// Fader ICaptionControl value label, gets part of custom IControl later
				pGraphics->AttachControl(new ICaptionControl(this, IRECT(
					properties.x,
					kHeight - textFg.mSize,
					properties.x + knob->W,
					kHeight
				), i, &textFg, true));

				// Fader control
				cControlMatrix.at(i) = pGraphics->AttachControl(new IFaderControl(this,
					properties.x,
					properties.y,
					kHeight - properties.y - 14, // fader lenght
					i, knob, kVertical, true));
				// ------------------------------------------------------------------
				// FADER

				break; // End double/int - faders


								// Unused
			case kEqHfQ:
			case kEqLfQ:
			case kTestParam1:
			case kTestParam2:
			case kTestParam3:
			case kTestParam4:
			case kTestParam5:
				break;

			default:									// ... but most of them get a nice rotary control

				// KNOBS
				// ------------------------------------------------------------------

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

				// ------------------------------------------------------------------
				// KNOBS

				break; // End double/int default 
			}
		default: // if no type: should not happen ...
			break;

		} // End typeSwitch

	} // End loop through params CreateGraphics

	AttachGraphics(pGraphics);
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
		GetGUI()->GetControl(cDeesserMeter)->SetTooltip("Gain reduction of Deesser");
		GetGUI()->GetControl(cOuputPeakMeter1)->SetTooltip("VU Meter of left output channel");
		GetGUI()->GetControl(cOutputPeakMeter2)->SetTooltip("VU Meter of right output channel");
		for (int i = 0; i < kNumParams; i++) {
			GetGUI()->GetControl(cControlMatrix.at(i))->SetTooltip(parameterProperties[i].tooltip);
			//Uncomment next line for showing control bounds in Debug mode
			//GetGUI()->ShowControlBounds(true);
		}
	}
}

void SRChannel::InitGain()
{
	mSampleRate = GetSampleRate();
	fInputGain.initGain(mInputGain, mInputGain, double(mSampleRate) / 10., false);
	fOutputGain.initGain(mOutputGain, mOutputGain, double(mSampleRate) / 10., false);
	fPan.initPan(SRPlugins::SRGain::typeSinusodial, mPan, true);
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
	fEqHfFilterL.setFilter(SRPlugins::SRFilters::biquad_highshelf, mEqHfFreq / mSampleRate, mEqHfQ, mEqHfGain, mSampleRate);
	fEqHmfFilterL.setFilter(SRPlugins::SRFilters::biquad_peak, mEqHmfFreq / mSampleRate, mEqHmfQ, mEqHmfGain, mSampleRate);
	fEqLmfFilterL.setFilter(SRPlugins::SRFilters::biquad_peak, mEqLmfFreq / mSampleRate, mEqLmfQ, mEqLmfGain, mSampleRate);
	fEqLfFilterL.setFilter(SRPlugins::SRFilters::biquad_lowshelf, mEqLfFreq / mSampleRate, mEqLfQ, mEqLfGain, mSampleRate);
	fDcBlockerL.setFc(10. / mSampleRate);


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
	fEqHfFilterR.setFilter(SRPlugins::SRFilters::biquad_highshelf, mEqHfFreq / mSampleRate, mEqHfQ, mEqHfGain, mSampleRate);
	fEqHmfFilterR.setFilter(SRPlugins::SRFilters::biquad_peak, mEqHmfFreq / mSampleRate, mEqHmfQ, mEqHmfGain, mSampleRate);
	fEqLmfFilterR.setFilter(SRPlugins::SRFilters::biquad_peak, mEqLmfFreq / mSampleRate, mEqLmfQ, mEqLmfGain, mSampleRate);
	fEqLfFilterR.setFilter(SRPlugins::SRFilters::biquad_lowshelf, mEqLfFreq / mSampleRate, mEqLfQ, mEqLfGain, mSampleRate);
	fDcBlockerR.setFc(10. / mSampleRate);


}

void SRChannel::InitCompPeak() {
	mSampleRate = GetSampleRate();
	fCompressorPeak.initCompressor(mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease, mCompPeakSidechainFilterFreq, mCompPeakKneeWidthDb, mSampleRate);
	fCompressorPeak.initRuntime();
}

void SRChannel::InitCompRms() {
	mSampleRate = GetSampleRate();
	// For sidechain filter frequency it requires an own knob later
	fCompressorRms.initCompressor(mCompRmsThresh, mCompRmsRatio, mCompRmsAttack, mCompRmsRelease, mCompPeakSidechainFilterFreq, mCompRmsKneeWidthDb, 300., mSampleRate);
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
	fDeesser.setDeesser(mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, mDeesserFreq / mSampleRate, mDeesserQ, 10., mSampleRate);
	fDeesser.initRuntime();
}

void SRChannel::InitMeter() {
	mSampleRate = GetSampleRate();
	fOutputVuMeterEnvelope1.setSampleRate(mSampleRate), fOutputVuMeterEnvelope2.setSampleRate(mSampleRate);
	fOutputVuMeterEnvelopeDetector1.setSampleRate(mSampleRate), fOutputVuMeterEnvelopeDetector2.setSampleRate(mSampleRate);
	fOutputVuMeterEnvelope1.setAttack(300.), fOutputVuMeterEnvelope2.setAttack(300.);
	fOutputVuMeterEnvelope1.setRelease(300.), fOutputVuMeterEnvelope2.setRelease(300.);
	fOutputVuMeterEnvelopeDetector1.setTc(5.), fOutputVuMeterEnvelopeDetector2.setTc(5.);
}

void SRChannel::InitSaturation() {
	fInputSaturation.setSaturation(SRPlugins::SRSaturation::typeMusicDSP, mInputDrive, mSaturationAmount, mSaturationHarmonics, false, mSaturationSkew, 1.);
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
	mDeesserMeterValue = 0., mDeesserMeterValue = 0.;
	mOutputPeakMeterValue1 = 0., mOutputPeakMeterValue2 = 0.;


	// Begin Processing per Frame

	for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2) {

		// Begin global bypass test
		// ------------------------

		if (mBypass == 1) {
			*out1 = *in1;
			*out2 = *in2;
		}
		else {

			// PRE SECTIONS
			// ------------

			// Adding DC Off
			*out1 = *in1 + dcoff;
			*out2 = *in2 + dcoff;


			// Input Gain
			if (mInputGain != 1.) {
				//*out1 *= mInputGain;
				//*out2 *= mInputGain;
			}
			fInputGain.process(*out1, *out2);

			// Input Meter
			mInputPeakMeterValue1 = IPMAX(mInputPeakMeterValue1, fabs(*out1));
			mInputPeakMeterValue2 = IPMAX(mInputPeakMeterValue2, fabs(*out2));

			circularBufferInL[circularBufferPointer] = *out1; // Fill circular buffer with input gain values
			circularBufferInR[circularBufferPointer] = *out2;

			// ----------------
			// End Pre Sections



			// INPUT SECTION
			// -------------
			if (mInputBypass != 1) {

				// Upsample
				// Insert upsample method here ...
				// double y = mOverSampler.Process(x, [](double input) { return std::tanh(input); })

				// Saturation
				if (mSaturationAmount != 0.) fInputSaturation.process(*out1, *out2);

				// Downsample
				// Insert downsample method here ...

			}
			//	----------------
			// End Input Section



			// EQ SECTION
			// ----------

			if (mEqBypass != 1) {

				// Filter

				// High Pass

				if (mEqHpFreq > 16.) {
					// one pole for 1st and 3rd (and further odd) orders
					if (mEqHpOrder == dbo6 || mEqHpOrder == dbo18) {
						*out1 = fEqHpFilterOnepoleL.process(*out1);
						*out2 = fEqHpFilterOnepoleR.process(*out2);
					}
					// two pole for 2nd order
					if (mEqHpOrder >= dbo12) {
						*out1 = fEqHpFilter1L.process(*out1);
						*out2 = fEqHpFilter1R.process(*out2);
					}
					// two pole for 4th order
					if (mEqHpOrder >= dbo24) {
						*out1 = fEqHpFilter2L.process(*out1);
						*out2 = fEqHpFilter2R.process(*out2);
					}
					// two pole for 6th order
					if (mEqHpOrder >= dbo36) {
						*out1 = fEqHpFilter3L.process(*out1);
						*out2 = fEqHpFilter3R.process(*out2);
					}
					// two pole for 8th order
					if (mEqHpOrder >= dbo48) {
						*out1 = fEqHpFilter4L.process(*out1);
						*out2 = fEqHpFilter4R.process(*out2);
					}
					// two pole for 10th order
					if (mEqHpOrder >= dbo60) {
						*out1 = fEqHpFilter5L.process(*out1);
						*out2 = fEqHpFilter5R.process(*out2);
					}
					// two pole for 12th order
					if (mEqHpOrder >= dbo72) {
						*out1 = fEqHpFilter6L.process(*out1);
						*out2 = fEqHpFilter6R.process(*out2);
					}
					// two pole for 20th order
					if (mEqHpOrder >= dbo120) {
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

				// Low Pass

				if (mEqLpFreq < 22000.0) { *out1 = fEqLpFilter1L.process(*out1); *out2 = fEqLpFilter1R.process(*out2); }

				// Parametric EQ

				if (mEqLfGain != 0.0) { *out1 = fEqLfFilterL.process(*out1); *out2 = fEqLfFilterR.process(*out2); }
				if (mEqLmfGain != 0.0) { *out1 = fEqLmfFilterL.process(*out1); *out2 = fEqLmfFilterR.process(*out2); }
				if (mEqHmfGain != 0.0) { *out1 = fEqHmfFilterL.process(*out1); *out2 = fEqHmfFilterR.process(*out2); }
				if (mEqHfGain != 0.0) { *out1 = fEqHfFilterL.process(*out1); *out2 = fEqHfFilterR.process(*out2); }
			}
			// --------------
			// End EQ Section



			// COMPRESSOR SECTION
			// ------------------

			if (mCompBypass != 1) {

				// Deesser

				if (mDeesserRatio != 1.0 && mDeesserThresh != 0.0) {
					fDeesser.process(*out1, *out2);
				}

				// Compressor

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

				// Compressor's Dry/Wet
				if (mCompDryWet != 1.) {
					*out1 = (mCompDryWet * *out1) + (vCompDry1 * (1. - mCompDryWet));
					*out2 = (mCompDryWet * *out2) + (vCompDry2 * (1. - mCompDryWet));
				}

			}
			// ----------------------
			// End Compressor Section



			// OUTPUT SECTION
			// --------------

			if (mOutputBypass != 1) {

				// Pan

				if (mPan != .5 || mIsPanMonoLow == true) {
					vSafePanLowSignal1 = *out1;
					vSafePanHighSignal1 = *out1;
					vSafePanLowSignal2 = *out2;
					vSafePanHighSignal2 = *out2;

					vSafePanLowSignal1 = fSafePanLpL.process(vSafePanLowSignal1);
					vSafePanLowSignal2 = fSafePanLpR.process(vSafePanLowSignal2);
					vSafePanHighSignal1 = fSafePanHpL.process(vSafePanHighSignal1);
					vSafePanHighSignal2 = fSafePanHpR.process(vSafePanHighSignal2);

					fPan.process(vSafePanHighSignal1, vSafePanHighSignal2);
					if (mIsPanMonoLow) {
						vSafePanLowSignal1 = (vSafePanLowSignal1 + vSafePanLowSignal2) * 0.5;
						vSafePanLowSignal2 = (vSafePanLowSignal1 + vSafePanLowSignal2) * 0.5;
					}
					*out1 = vSafePanHighSignal1 - vSafePanLowSignal1;
					*out2 = vSafePanHighSignal2 - vSafePanLowSignal2;
				}


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
			// ------------------
			// End Output Section



			// POST SECTIONS
			// -------------

			// Fill circular buffer with output gain values
			circularBufferOutL[circularBufferPointer] = *out1;
			circularBufferOutR[circularBufferPointer] = *out2;

			// Output Gain
			fOutputGain.process(*out1, *out2);
			//if (mOutputGain != 1.) {
			//	*out1 *= mOutputGain;
			//	*out2 *= mOutputGain;
			//}

			*out1 = fDcBlockerL.process(*out1);
			*out2 = fDcBlockerR.process(*out2);
			*out1 -= dcoff;
			*out2 -= dcoff;

			// ----------------
			// End Post Section


		}
		// -------------------------
		// End of global bypass test



		// Output Meter +  GR Meter

		mOutputPeakMeterValue1 = IPMAX(mOutputPeakMeterValue1, fabs(*out1));
		mOutputPeakMeterValue2 = IPMAX(mOutputPeakMeterValue2, fabs(*out2));

		double limitedOut1 = (*out1 < DBToAmp(-38.)) ? DBToAmp(-38.) : *out1;
		double limitedOut2 = (*out2 < DBToAmp(-38.)) ? DBToAmp(-38.) : *out2;
		fOutputVuMeterEnvelopeDetector1.run(fabs(limitedOut1), mOutputVuMeterSos1);
		fOutputVuMeterEnvelopeDetector2.run(fabs(limitedOut2), mOutputVuMeterSos2);
		fOutputVuMeterEnvelope1.run(AmpToDB(mOutputVuMeterSos1), mOutputVuMeterValue1);
		fOutputVuMeterEnvelope2.run(AmpToDB(mOutputVuMeterSos2), mOutputVuMeterValue2);

		(circularBufferPointer >= circularBufferLenght - 1) ? circularBufferPointer = 0 : circularBufferPointer++;

	}
	// ----------------------------------------------------------------
	// End processing per frame and again some processing per Framesize


	// Get input peak meter value from maxed values each sample
	mInputPeakMeterTimeConst1 = (mInputPeakMeterValue1 < mInputPeakMeterPreviousValue1 ? METER_DECAY : METER_ATTACK);
	mInputPeakMeterTimeConst2 = (mInputPeakMeterValue2 < mInputPeakMeterPreviousValue2 ? METER_DECAY : METER_ATTACK);
	mInputPeakMeterValue1 = mInputPeakMeterValue1 * mInputPeakMeterTimeConst1 + mInputPeakMeterPreviousValue1 * (1.0 - mInputPeakMeterTimeConst1);
	mInputPeakMeterValue2 = mInputPeakMeterValue2 * mInputPeakMeterTimeConst2 + mInputPeakMeterPreviousValue2 * (1.0 - mInputPeakMeterTimeConst2);
	mInputPeakMeterPreviousValue1 = mInputPeakMeterValue1;
	mInputPeakMeterPreviousValue2 = mInputPeakMeterValue2;
	mInputPeakMeterValue1 = (AmpToDB(mInputPeakMeterValue1) + 60) / 60;
	mInputPeakMeterValue2 = (AmpToDB(mInputPeakMeterValue2) + 60) / 60;

	// Get output peak meter value from maxed values each sample
	mOutputPeakMeterTimeConst1 = (mOutputPeakMeterValue1 < mOutputPeakMeterPreviousValue1 ? METER_DECAY : METER_ATTACK);
	mOutputPeakMeterTimeConst2 = (mOutputPeakMeterValue2 < mOutputPeakMeterPreviousValue2 ? METER_DECAY : METER_ATTACK);
	mOutputPeakMeterValue1 = mOutputPeakMeterValue1 * mOutputPeakMeterTimeConst1 + mOutputPeakMeterPreviousValue1 * (1.0 - mOutputPeakMeterTimeConst1);
	mOutputPeakMeterValue2 = mOutputPeakMeterValue2 * mOutputPeakMeterTimeConst2 + mOutputPeakMeterPreviousValue2 * (1.0 - mOutputPeakMeterTimeConst2);
	mOutputPeakMeterPreviousValue1 = mOutputPeakMeterValue1;
	mOutputPeakMeterPreviousValue2 = mOutputPeakMeterValue2;
	mOutputPeakMeterValue1 = (AmpToDB(mOutputPeakMeterValue1) + 60) / 60;
	mOutputPeakMeterValue2 = (AmpToDB(mOutputPeakMeterValue2) + 60) / 60;

	// Update gain reduction meters with current state after processing block
	mRmsGrMeterValue = (mCompBypass != 1 && mCompRmsRatio != 1. && mCompRmsThresh != 0. && mBypass != 1)
		? (-fabs(fCompressorRms.getGrDb()) + 60) / 60 : 1.;
	mPeakGrMeterValue = (mCompBypass != 1 && mCompPeakRatio != 1. && mCompPeakThresh != 0. && mBypass != 1)
		? (-fabs(fCompressorPeak.getGrDb()) + 60) / 60 : 1.;
	mDeesserMeterValue = (mCompBypass != 1 && mDeesserRatio != 1. && mDeesserThresh != 0. && mBypass != 1)
		? (-fabs(fDeesser.getGrDb()) + 60) / 60 : 1.;

	// Set meter GUIs from plug
	if (GetGUI())
	{
		GetGUI()->SetControlFromPlug(cInputPeakMeter1, mInputPeakMeterValue1);
		GetGUI()->SetControlFromPlug(cInputPeakMeter2, mInputPeakMeterValue2);
		GetGUI()->SetControlFromPlug(cRmsGrMeter, mRmsGrMeterValue);
		GetGUI()->SetControlFromPlug(cPeakGrMeter, mPeakGrMeterValue);
		GetGUI()->SetControlFromPlug(cDeesserMeter, mDeesserMeterValue);
		GetGUI()->SetControlFromPlug(cOuputPeakMeter1, mOutputPeakMeterValue1);
		GetGUI()->SetControlFromPlug(cOutputPeakMeter2, mOutputPeakMeterValue2);
		GetGUI()->SetControlFromPlug(cOutputVuMeter1, ((mOutputVuMeterValue1 + 60) / 60));
		GetGUI()->SetControlFromPlug(cOutputVuMeter2, ((mOutputVuMeterValue2 + 60) / 60));
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
	InitGain();
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
		// ----------------------

	case kInputGain:
		mInputGain = DBToAmp(GetParam(paramIdx)->Value());
		fInputGain.setGain(mInputGain);
		break;
	case kOutputGain:
		mOutputGain = DBToAmp(GetParam(paramIdx)->Value());
		fOutputGain.setGain(mOutputGain);
		break;
	case kInputDrive:
		mInputDrive = GetParam(paramIdx)->Value();
		fInputSaturation.setDrive(mInputDrive);
		break;
	case kSaturationAmount:
		mSaturationAmount = GetParam(paramIdx)->Value() / 100.;
		fInputSaturation.setAmount(mSaturationAmount);
		break;
	case kSaturationHarmonics:
		mSaturationHarmonics = GetParam(paramIdx)->Value() / 100.;
		fInputSaturation.setHarmonics(mSaturationHarmonics);
		break;
	case kSaturationSkew:
		mSaturationSkew = GetParam(paramIdx)->Value() * 0.05;
		fInputSaturation.setSkew(mSaturationSkew);
		break;
	case kSaturationType:
		mSaturationType = GetParam(paramIdx)->Value();
		fInputSaturation.setType(mSaturationType);
		break;
	case kClipperThreshold: mClipperThreshold = 1. - GetParam(paramIdx)->Value() / 100.; break;
	case kLimiterThresh:
		mLimiterThresh = GetParam(paramIdx)->Value();
		fLimiter.setThresh(mLimiterThresh);
		break;
	case kPan: mPan = (GetParam(paramIdx)->Value() + 100) / 200; fPan.setPanPosition(mPan); break;
	case kPanFreq:
		mSafePanFreq = GetParam(paramIdx)->Value();
		fSafePanHpL.setFc(mSafePanFreq / mSampleRate);
		fSafePanHpR.setFc(mSafePanFreq / mSampleRate);
		fSafePanLpL.setFc(mSafePanFreq / mSampleRate);
		fSafePanLpR.setFc(mSafePanFreq / mSampleRate);
		break;
	case kIsPanMonoLow: mIsPanMonoLow = GetParam(paramIdx)->Value(); break;


		// FILTER
		// ------

		// High Pass
	case kEqHpFreq:
		mEqHpFreq = GetParam(paramIdx)->Value();
		if (mEqHpOrder == dbo6 || mEqHpOrder == dbo18) {
			fEqHpFilterOnepoleL.setFc(mEqHpFreq / mSampleRate); fEqHpFilterOnepoleR.setFc(mEqHpFreq / mSampleRate);
		}
		if (mEqHpOrder >= dbo12) {
			fEqHpFilter1L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter1R.setFc(mEqHpFreq / mSampleRate);
		}
		if (mEqHpOrder >= dbo24) {
			fEqHpFilter2L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter2R.setFc(mEqHpFreq / mSampleRate);
		}
		if (mEqHpOrder >= dbo36) {
			fEqHpFilter3L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter3R.setFc(mEqHpFreq / mSampleRate);
		}
		if (mEqHpOrder >= dbo48) {
			fEqHpFilter4L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter4R.setFc(mEqHpFreq / mSampleRate);
		}
		if (mEqHpOrder >= dbo60) {
			fEqHpFilter5L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter5R.setFc(mEqHpFreq / mSampleRate);
		}
		if (mEqHpOrder >= dbo72) {
			fEqHpFilter6L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter6R.setFc(mEqHpFreq / mSampleRate);
		}
		if (mEqHpOrder >= dbo120) {
			fEqHpFilter7L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter7R.setFc(mEqHpFreq / mSampleRate);
			fEqHpFilter8L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter8R.setFc(mEqHpFreq / mSampleRate);
			fEqHpFilter9L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter9R.setFc(mEqHpFreq / mSampleRate);
			fEqHpFilter10L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter10R.setFc(mEqHpFreq / mSampleRate);
		}
		break;

	case kEqHpOrder:
		mEqHpOrder = int(GetParam(paramIdx)->Value());
		switch (mEqHpOrder) {
		case dbo6:									// 1st order, 6 dB/Oct
			break;
		case dbo12:
			fEqHpFilter1L.setQ(mEqPassQ_O2_F1);	// 2nd order, 12 dB/Oct
			fEqHpFilter1R.setQ(mEqPassQ_O2_F1);
			break;
		case dbo18:									// 3rd order, 18 dB/Oct
			fEqHpFilter1L.setQ(mEqPassQ_O3_F1);
			fEqHpFilter1R.setQ(mEqPassQ_O3_F1);
			break;
		case dbo24:									// 4th order, 24 dB/Oct
			fEqHpFilter1L.setQ(mEqPassQ_O4_F1);
			fEqHpFilter1R.setQ(mEqPassQ_O4_F1);
			fEqHpFilter2L.setQ(mEqPassQ_O4_F2);
			fEqHpFilter2R.setQ(mEqPassQ_O4_F2);
			break;
		case dbo36:									// 6th order, 36 dB/Oct
			fEqHpFilter1L.setQ(mEqPassQ_O6_F1);
			fEqHpFilter1R.setQ(mEqPassQ_O6_F1);
			fEqHpFilter2L.setQ(mEqPassQ_O6_F2);
			fEqHpFilter2R.setQ(mEqPassQ_O6_F2);
			fEqHpFilter3L.setQ(mEqPassQ_O6_F3);
			fEqHpFilter3R.setQ(mEqPassQ_O6_F3);
			break;
		case dbo48:									// 8th order, 48 dB/Oct
			fEqHpFilter1L.setQ(mEqPassQ_O8_F1);
			fEqHpFilter1R.setQ(mEqPassQ_O8_F1);
			fEqHpFilter2L.setQ(mEqPassQ_O8_F2);
			fEqHpFilter2R.setQ(mEqPassQ_O8_F2);
			fEqHpFilter3L.setQ(mEqPassQ_O8_F3);
			fEqHpFilter3R.setQ(mEqPassQ_O8_F3);
			fEqHpFilter4L.setQ(mEqPassQ_O8_F4);
			fEqHpFilter4R.setQ(mEqPassQ_O8_F4);
			break;
		case dbo60:									// 10th order, 48 dB/Oct
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
		case dbo72:									// 12th order, 72 db/Oct
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
		case dbo120:									// 20th order, 120 dB/Oct
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

		// Low Pass
	case kEqLpOrder:
		break;

	case kEqLpFreq:
		mEqLpFreq = GetParam(paramIdx)->Value();
		fEqLpFilter1L.setFc(mEqLpFreq / mSampleRate);
		fEqLpFilter1R.setFc(mEqLpFreq / mSampleRate);
		break;


		// EQUALIZER
		// ---------

	case kEqHfBell:
		mEqHfIsBell = GetParam(paramIdx)->Value();
		if (mEqHfIsBell == 1) { fEqHfFilterL.setType(SRPlugins::SRFilters::biquad_peak); fEqHfFilterR.setType(SRPlugins::SRFilters::biquad_peak); }
		else { fEqHfFilterL.setType(SRPlugins::SRFilters::biquad_highshelf); fEqHfFilterR.setType(SRPlugins::SRFilters::biquad_highshelf); }
		break;

	case kEqLfBell:
		mEqLfIsBell = GetParam(paramIdx)->Value();
		if (mEqLfIsBell == 1) { fEqLfFilterL.setType(SRPlugins::SRFilters::biquad_peak); fEqLfFilterR.setType(SRPlugins::SRFilters::biquad_peak); }
		else { fEqLfFilterL.setType(SRPlugins::SRFilters::biquad_lowshelf); fEqLfFilterR.setType(SRPlugins::SRFilters::biquad_lowshelf); }
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
		// ----------

		// Peak Compressor
	case kCompPeakRatio:
		mCompPeakRatio = (1. / GetParam(paramIdx)->Value());
		if (mCompPeakRatio <= 1. / 20.) {
			mCompPeakRatio = 0.;
		}
		fCompressorPeak.setRatio(mCompPeakRatio);
		mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
		break;

	case kCompPeakThresh:
		mCompPeakThresh = GetParam(paramIdx)->Value();
		fCompressorPeak.setThresh(mCompPeakThresh);
		mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
		break;

	case kCompPeakAttack:
		mCompPeakAttack = GetParam(paramIdx)->Value();
		fCompressorPeak.setAttack(mCompPeakAttack);
		mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
		break;

	case kCompPeakRelease:
		mCompPeakRelease = GetParam(paramIdx)->Value();
		fCompressorPeak.setRelease(mCompPeakRelease);
		mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
		break;

	case kCompPeakKneeWidthDb:
		mCompPeakKneeWidthDb = GetParam(paramIdx)->Value();
		fCompressorPeak.setKnee(mCompPeakKneeWidthDb);
		break;

	case kCompPeakSidechainFilterFreq:
		mCompPeakSidechainFilterFreq = GetParam(paramIdx)->Value();
		fCompressorPeak.setSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate);
		fCompressorRms.setSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate); // !!! This moves to own switch when RMS sidechain filter is implemented
		break;

	case kCompPeakMakeup: mCompPeakMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;


		// RMS Compressor
	case kCompRmsRatio:
		mCompRmsRatio = (1 / GetParam(paramIdx)->Value());
		fCompressorRms.setRatio(mCompRmsRatio);
		mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
		break;

	case kCompRmsThresh:
		mCompRmsThresh = GetParam(paramIdx)->Value();
		fCompressorRms.setThresh(mCompRmsThresh);
		mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
		break;

	case kCompRmsAttack:
		mCompRmsAttack = GetParam(paramIdx)->Value();
		fCompressorRms.setAttack(mCompRmsAttack);
		mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
		break;

	case kCompRmsRelease:
		mCompRmsRelease = GetParam(paramIdx)->Value();
		fCompressorRms.setRelease(mCompRmsRelease);
		mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
		break;

	case kCompRmsKneeWidthDb: mCompRmsKneeWidthDb = GetParam(paramIdx)->Value(); fCompressorRms.setKnee(mCompRmsKneeWidthDb); break;
	case kCompRmsMakeup: mCompRmsMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;

		// Both Compressors
	case kCompPeakRmsRatio: mCompPeakRmsRatio = GetParam(paramIdx)->Value() / 100; break;
	case kCompDryWet: mCompDryWet = GetParam(paramIdx)->Value() / 100; break;

		// Bool Switches
	case kCompIsParallel: mCompIsParallel = GetParam(paramIdx)->Value(); if (GetGUI()) GrayOutControls(); break;
	case kCompPeakIsExtSc: mCompPeakIsExtSc = GetParam(paramIdx)->Value(); break;
	case kCompRmsIsExrSc: mCompRmsIsExtSc = GetParam(paramIdx)->Value(); break;


		// GLOBAL BYPASS
		// -------------

	case kEqBypass: mEqBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
	case kCompBypass: mCompBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
	case kInputBypass: mInputBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
	case kOutputBypass: mOutputBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
	case kBypass: mBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;


		// AUTOMATIC GAIN CONTROL
		// ----------------------

	case kAgc:
		if (circularBufferInL[circularBufferLenght - 1]) {
			sumIn = 0.;
			sumOut = 0.;
			aveIn = 0.;
			aveOut = 0.;
			diffInOut = 0.;
			for (int i = 0; i <= circularBufferLenght - 1; i++) {
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

		// DEESSER
		// -------

	case kDeesserFreq: mDeesserFreq = GetParam(paramIdx)->Value(); fDeesser.setFrequency(mDeesserFreq / mSampleRate); break;
	case kDeesserQ: mDeesserQ = GetParam(paramIdx)->Value(); fDeesser.setQ(mDeesserQ); break;
	case kDeesserThresh: mDeesserThresh = GetParam(paramIdx)->Value(); fDeesser.setThresh(mDeesserThresh); break;
	case kDeesserRatio: mDeesserRatio = (1. / GetParam(paramIdx)->Value()); fDeesser.setRatio(mDeesserRatio); break;
	case kDeesserAttack: mDeesserAttack = GetParam(paramIdx)->Value(); fDeesser.setAttack(mDeesserAttack); break;
	case kDeesserRelease: mDeesserRelease = GetParam(paramIdx)->Value(); fDeesser.setRelease(mDeesserRelease); break;
	case kDeesserMakeup: mDeesserMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;

		// TEST PARAMS
		// -----------

	case kTestParam1: mTestParam1 = GetParam(paramIdx)->Value(); break;
	case kTestParam2: mTestParam2 = GetParam(paramIdx)->Value(); break;
	case kTestParam3: mTestParam3 = GetParam(paramIdx)->Value(); break;
	case kTestParam4: mTestParam4 = GetParam(paramIdx)->Value(); break;
	case kTestParam5: mTestParam5 = GetParam(paramIdx)->Value(); break;

	default: break;
	}

}