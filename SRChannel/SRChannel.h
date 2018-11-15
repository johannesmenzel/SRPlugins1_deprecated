/*
SRChannel by Johannes Menzel

contributions: 
Compressor Algorithm by Bojan Markovic (https://github.com/music-dsp-collection/chunkware-simple-dynamics/tree/master/simpleSource)
Biquad Source by Nigel Redmon (http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/)
*/

#ifndef __SRCHANNEL__
#define __SRCHANNEL__

#include "IPlug_include_in_plug_hdr.h"
#include "../SRClasses/SRFilters.h"
//#include "../SRClasses/SROnePole.h"
//#include "../SRClasses/SimpleHeader.h"
//#include "../SRClasses/SimpleComp.h"
//#include "../SRClasses/SimpleLimit.h"
#include "../SRClasses/SRDynamics.h"
#include "../SRClasses/SRHelpers.h"
#include "../SRClasses/SRControls.h"
#include "../SRClasses/Oversampler.h"
#include "resample.h"
#include "denormal.h"
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
//#include "SRGraphics.h"
//#include "../SRClasses/SRDynamics.h"




	class SRChannel : public IPlug
	{
	public:
		SRChannel(IPlugInstanceInfo instanceInfo);
		~SRChannel();

		void Reset();
		void OnParamChange(int paramIdx);
		void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
		// CIRCULAR BUFFER
		//double circularBuffer[4][65536];
		double circularBufferInL[65536], circularBufferInR[65536], circularBufferOutL[65536], circularBufferOutR[65536];

	private:
		// GAIN + PAN
		double mInputGain, mOutputGain, mInputDrive,
			mPan, mSafePanFreq,
			vSafePanLowSignal1, vSafePanHighSignal1, vSafePanLowSignal2, vSafePanHighSignal2,

			// CLIP + LIMIT
			mClipperThreshold, mSaturationThreshold, mSaturationHarmonics, mLimiterThresh,

			//mSatLfGain, mSatLfFreq, mSatMfGain, mSatMfFreq, mSatHfGain, mSatHfFreq, mSatHpFreq, mSatLpFreq,

			// EQ
			mEqHpFreq, mEqLpFreq,
			mEqLfGain, mEqLfFreq, mEqLfQ,
			mEqLmfGain, mEqLmfFreq, mEqLmfQ,
			mEqHmfGain, mEqHmfFreq, mEqHmfQ,
			mEqHfGain, mEqHfFreq, mEqHfQ,

			// COMP
			mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease,
			mCompRmsThresh, mCompRmsRatio, mCompRmsAttack, mCompRmsRelease,
			mCompRmsMakeup, mCompPeakMakeup, mCompRmsAutoMakeup, mCompPeakAutoMakeup,
			mCompPeakRmsRatio, mCompDryWet,
			mCompPeakSidechainFilterFreq,
			mCompPeakKneeWidthDb, mCompRmsKneeWidthDb,

			vCompDry1, vCompDry2,
			vCompRmsIn1, vCompPeakIn1, vCompRmsIn2, vCompPeakIn2,

			// DEESSER
			mDeesserBottomFreq, mDeesserTopFreq,
			mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, mDeesserMakeup,

			vDeesserLowSignalL, vDeesserLowSignalR, vDeesserMidSignalL, vDeesserMidSignalR, vDeesserHighSignalL, vDeesserHighSignalR,


			// TESTPARAM
			mTestParam1, mTestParam2, mTestParam3, mTestParam4, mTestParam5,

			// METERS
			meterInputPeak1, meterInputPeak2,
			mCompRmsGr, mCompPeakGr,
			meterOutputPeak1, meterOutputPeak2,

			meterInputPrev1, meterInputPrev2, meterOutputPrev1, meterOutputPrev2,
			xMeterInput1, xMeterInput2, xMeterOutput1, xMeterOutput2;

		// FUNCT
		//SetShapeCentered(char csetshapeparam, double cCenteredValue, double cControlPosition),

		// FREQRESP
		//meterFreqRespValues[64];


	// BOOL VARS
		bool mEqLfIsBell, mEqHfIsBell, mCompIsParallel, mEqBypass, mCompBypass, mInputBypass, mOutputBypass, mBypass,
			mAgc;

		// INT VARS
		int mEqHpOrder,

			// METERS
			meterInput1, meterInput2, meterOutput1, meterOutput2,
			meterGrRms, meterGrPeak, meterFreqResp;


		// CIRCULAR BUFFER
		unsigned short int circularBufferPointer;


		// FUNCTIONS
		void CreatePresets(),
			CreateParams(),
			CreateGraphics(),
			InitGUI(),
			InitBiquad(),
			InitCompPeak(),
			InitCompRms(),
			InitDeesser(),
			InitLimiter(),
			InitSafePan();
		//CalculateFreqResp(),

	// FILTERS
	// Spectral Filters
		SRPlugins::SRFilters::SRFiltersTwoPole fEqHpFilter1L, fEqHpFilter2L, fEqHpFilter3L, fEqHpFilter4L, fEqHpFilter5L, fEqHpFilter6L, fEqHpFilter7L, fEqHpFilter8L, fEqHpFilter9L, fEqHpFilter10L,
			fEqLpFilter1L,
			fEqLfFilterL, fEqLmfFilterL, fEqHmfFilterL, fEqHfFilterL,
			fEqHpFilter1R, fEqHpFilter2R, fEqHpFilter3R, fEqHpFilter4R, fEqHpFilter5R, fEqHpFilter6R, fEqHpFilter7R, fEqHpFilter8R, fEqHpFilter9R, fEqHpFilter10R,
			fEqLpFilter1R,
			fEqLfFilterR, fEqLmfFilterR, fEqHmfFilterR, fEqHfFilterR,
			// Safe-Pan
			fSafePanHpL, fSafePanLpL, fSafePanHpR, fSafePanLpR,

			// Deesser
			fDeesserBottomLpFilterL, fDeesserBottomLpFilterR, fDeesserMidHpFilterL, fDeesserMidHpFilterR, fDeesserMidLpFilterL, fDeesserMidLpFilterR, fDeesserTopHpFilterL, fDeesserTopHpFilterR;
		//fSatHpFilterL, fSatHpFilterR, fSatLfFilterL, fSatLfFilterR, fSatMfFilterL, fSatMfFilterR, fSatHfFilterL, fSatHfFilterR, fSatLpFilterL, fSatLpFilterR // These were the filters to alter the saturated sound

		SRPlugins::SRFilters::SRFiltersOnePole fDcBlockerL, fDcBlockerR, fEqHpFilterOnepoleL, fEqHpFilterOnepoleR, fEqLpFilterOnepoleL, fEqLpFilterOnepoleR;

		// Dynamic Filters
		SRPlugins::SRDynamics::SRCompressor fCompressorPeak;
		SRPlugins::SRDynamics::SRCompressorRMS fCompressorRms;
		SRPlugins::SRDynamics::SRLimiter fLimiter;
		SRPlugins::SRDynamics::SRCompressor fDeesser;

		// TESTVARS
		double sumIn;
		double sumOut;
		double aveIn;
		double aveOut;
		double diffInOut;

		int cCompPeakRmsRatio;
	};

#endif

