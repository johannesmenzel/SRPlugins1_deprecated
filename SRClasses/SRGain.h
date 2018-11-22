//  SRGain.h

// Implementation:
//   Note that you might need a class for each channel
//   (so 2 for stereo processing.)
//
// Header:
//   private:
//     Impementation as object:
//       SRPan name;
//     Implementation as pointer:
//       SRPan *name = new SRPan();
//
// Class: Constructor, Reset()
//     Implementation as object:
//       name.setClass(pType, pVar1, pVar2, pVar3);
//     Implementation as pointer:
//       name->setClass(pType, pVar1, pVar2, pVar3);
//
// Class: ProcessDoubleReplacing()
//   Per sample and channel:
//     Implementation as object:
//       *out1 = name.process(*in1);
//     Implementation as pointer:
//       *out1 = name->process(*in1);

#ifndef SRGain_h
#define SRGain_h

// #include this and that
#include "SRHelpers.h" // optional
#include <cassert>

namespace SRPlugins
{
	namespace SRGain
	{


		class SRGain
		{
		public:
			SRGain();
			SRGain(double pGain1, double pGain2, double pRampNumSamples, bool pBypassed);
			~SRGain();

			void setGain(double pGainLin);
			void setGain(double pGainLin1, double pGainLin2);
			void setGainDb(double pGainDb);
			void setGainDb(double pGainDb1, double pGainDb2);
			void setRamp(double pRampNumSamples);
			void setBypassed(bool pBypassed);
			double getGain() { return mGainLin1 * 0.5 + mGainLin2 * 0.5; }
			double getGainDb() { return SRPlugins::SRHelpers::AmpToDB(mGainLin1 * 0.5 + mGainLin2 * 0.5); }
			bool getBypassed() { return mBypassed; }


			void initGain(double pGainLin1, double pGainLin2, double pRampNumSamples, bool pBypassed);
			void initGainDb(double pGainDb1, double pGainDb2, double pRampNumSamples, bool pBypassed);

			// void process(double &in);			// for mono
			void process(double &in1, double &in2); // for stereo

		protected:
			void calcGain(void);

			double mGainLin1; // Enter normalized gain >= 0.0 (1.0 = unity)
			double mGainLin2; // Enter normalized gain of second channel >= 0.0 (1.0 = unity)
			double mRampNumSamples; // Enter lenght of gain ramp in samples
			bool mBypassed; // bool is gain bypassed

			double currentGain1, currentGain2, targetGain1, targetGain2, stepGain1, stepGain2;
		};



		inline void SRGain::process(double &in1, double &in2)
		{
			if (currentGain1 != 1.0 && stepGain1 != 0.0 && mBypassed == false) {
				if (stepGain1 != 0.0) {
					currentGain1 += stepGain1;
					if ((stepGain1 > 0.0 && currentGain1 > targetGain1) || (stepGain1 < 0.0 && currentGain1 < targetGain1)) {
						currentGain1 = targetGain1;
						stepGain1 = 0.0;
					}
				}
				in1 *= currentGain1;
			}

			if (currentGain1 != 1.0 && stepGain1 != 0.0 && mBypassed == false) {
				if (stepGain2 != 0.0) {
					currentGain2 += stepGain2;
					if ((stepGain2 > 0.0 && currentGain2 > targetGain2) || (stepGain2 < 0.0 && currentGain2 < targetGain2)) {
						currentGain2 = targetGain2;
						stepGain2 = 0.0;
					}
				}
				in2 *= currentGain2;
			}

		}


		// If type definitions of type int needed:
		enum
		{
			typeLinear = 0,
			typeSquareroot,
			typeSinusodial,
			typeTanh,
			numTypes
			// ...
		};

		class SRPan
		{
		public:
			SRPan();
			SRPan(int pType, double pPanNormalized, bool pLinearMiddlePosition);
			~SRPan();

			void setType(int pType);
			void setLinearMiddlePosition(bool pLinearMiddlePosition);
			void setPanPosition(double pPanNormalized); // create these for every member
			double getPanPosition() { return mPanNormalized; }

			void initPan(int pType, double pPanNormalized, bool pLinearMiddlePosition);
			// void process(double &in);				// for mono
			void process(double &in1, double &in2); // for stereo

		protected:
			void calcPan(void);

			int mType;
			double mPanNormalized; // Enter normalized pan between 0.0 (left) and 1.0 (right)
			bool mLinearMiddlePosition;

			double gain1, gain2;
			SRGain panSmoother;
		};

		inline void SRPan::process(double &in1, double &in2)
		{
			if (mPanNormalized != 0.5)
			{
				panSmoother.process(in1, in2);
			}
		}

	} // namespace SRGain
} // namespace SRPlugins

#endif // SRGain_h