//  SRSaturation.h

// Implementation:
//   Note that you might need a class for each channel
//   (so 2 for stereo processing.)
//
// Header:
//   private:
//     Impementation as object:
//       SRSaturation name;
//     Implementation as pointer:
//       SRSaturation *name = new SRSaturation();
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

#ifndef SRSaturation_h
#define SRSaturation_h

#include "SRHelpers.h"
// #include this and that


namespace SRPlugins {
	namespace SRSaturation {


		// If type definitions of type int needed:
		enum SaturationTypes {
			typeMusicDSP = 0,
			typeZoelzer,
			typePirkle,
			typePirkleModified,
			typeRectHalf,
			typeRectFull,
			numTypes
			// ...
		};

		class SRSaturation {
		public:
			// constructor
			SRSaturation();
			// class initializer
			SRSaturation(
				int pType,
				double pDriveDb,
				double pAmountNormalized,
				double pHarmonicsNormalized,
				bool pPositiveSide,
				double pSkewNormalized,
				double pWet
			);
			// destructor
			~SRSaturation(); // destructor

			// public functions that need to be accessed from outside
			void setType(int pType);
			void setDrive(double pDriveDb);
			void setAmount(double pAmountNormalized);
			void setHarmonics(double pHarmonicsNormalized);
			void setPositive(bool pPositive);
			void setSkew(double pSkewNormalized);
			void setWet(double pWetNormalized);

			void setSaturation(
				int pType,
				double pDriveDb,
				double pAmountNormalized,
				double pHarmonicsNormalized,
				bool pPositive,
				double pSkewNormalized,
				double pWetNormalized
			);
			// inline process function, if needed
			void process(double &in1, double &in2);



		protected:
			// Protected functions that do internal calculations and that are called from other funcions
			void processMusicDSP(double &in1, double &in2);
			void processZoelzer(double &in1, double &in2);
			void processPirkle(double &in1, double &in2);
			void processPirkleModified(double &in1, double &in2);
			void processRectHalf(double &in1, double &in2);
			void processRectFull(double &in1, double &in2);
			void calcSaturation(void);

			// Internal member and internal variables
			int mType;
			// member variables
			double mDriveNormalized;
			double mAmountNormalized;
			double mAmount;
			double mHarmonicsNormalized;
			bool mPositive; // if aiming for even harmonics, the positive side of the envelope will be affected if true, otherwise the negative side
			double mSkewNormalized;
			double mWetNormalized;
			// internal variables
			double in1PrevSample;
			double in2PrevSample;
			double in1Dry;
			double in2Dry;

		}; // end of class

		inline void SRSaturation::process(double &in1, double &in2) {

			// apply drive
			in1 *= mDriveNormalized;
			in2 *= mDriveNormalized;

			// create driven dry samples
			in1Dry = in1;
			in2Dry = in2;

			// call specific inline functions
			switch (this->mType) {
			case typeMusicDSP: processMusicDSP(in1, in2); break;
			case typePirkle: processPirkle(in1, in2); break;
			case typeZoelzer: processZoelzer(in1, in2); break;
			case typePirkleModified: processPirkleModified(in1, in2); break;
			case typeRectHalf: processRectHalf(in1, in2); break;
			case typeRectFull: processRectFull(in1, in2); break;
			default: in1 = in1; in2 = in2; break;
			}

			if (!mPositive && in1 < 0.) in1 = in1 * mHarmonicsNormalized + in1Dry * (1. - mHarmonicsNormalized);
			if (mPositive && in1 > 0.) in1 = in1 * mHarmonicsNormalized + in1Dry * (1. - mHarmonicsNormalized);
			if (!mPositive && in2 < 0.) in2 = in2 * mHarmonicsNormalized + in2Dry * (1. - mHarmonicsNormalized);
			if (mPositive && in2 > 0.) in2 = in2 * mHarmonicsNormalized + in2Dry * (1. - mHarmonicsNormalized);

			in1PrevSample = in1Dry;
			in2PrevSample = in2Dry;

			// return to old drive level
			in1 *= (1. / mDriveNormalized);
			in2 *= (1. / mDriveNormalized);

		}

		inline void SRSaturation::processMusicDSP(double &in1, double &in2) {
			if (fabs(in1) > mAmount) {
				in1 = (in1 > 0.)
					? (mAmount + (fabs(in1) - mAmount) / (1. + pow((fabs(in1) - mAmount) / (1 - mAmount), 2))) * (in1 / fabs(in1))
					: (mAmount + (fabs(in1) - mAmount) / (1. + pow((fabs(in1) - mAmount) / (1 - mAmount), 2))) * (in1 / fabs(in1));
			}

			if (fabs(in2) > mAmount) {
				in2 = (in2 > 0)
					? (mAmount + (fabs(in2) - mAmount) / (1. + pow((fabs(in2) - mAmount) / (1 - mAmount), 2))) * (in2 / fabs(in2))
					: (mAmount + (fabs(in2) - mAmount) / (1. + pow((fabs(in2) - mAmount) / (1 - mAmount), 2))) * (in2 / fabs(in2));
			}

			// Soften by (1 - Amount)
			in1 = (1. - mAmount) * in1 + mAmount * in1Dry;
			in2 = (1. - mAmount) * in2 + mAmount * in2Dry;

			// Saturation Normalization
			in1 *= (1. / ((mAmount + 1.) / 2.));
			in2 *= (1. / ((mAmount + 1.) / 2.));
		}

		inline void SRSaturation::processZoelzer(double &in1, double &in2) {
			if (mAmountNormalized > 0.) {
				in1 = (in1 > 0.)
					? (1 - exp(-in1)) * mAmountNormalized + in1Dry * (1. - mAmountNormalized)
					: ((-1 + exp(in1)) * mAmountNormalized + in1Dry * (1. - mAmountNormalized));

				in2 = (in2 > 0.)
					? (1 - exp(-in2)) * mAmountNormalized + in2Dry * (1. - mAmountNormalized)
					: ((-1 + exp(in2)) * mAmountNormalized + in2Dry * (1. - mAmountNormalized));
			}

		}

		inline void SRSaturation::processPirkle(double &in1, double &in2) {

			if (mAmountNormalized > .001) {
				double mAmountModified = pow(mAmountNormalized, 3.);
				in1 = (in1 >= 0)
					? tanh(mAmountModified * in1) / tanh(mAmountModified)
					: tanh(mAmountModified * in1) / tanh(mAmountModified);

				in2 = (in2 >= 0.)
					? tanh(mAmountModified * in2) / tanh(mAmountModified)
					: tanh(mAmountModified * in2) / tanh(mAmountModified);
			}
		}

		inline void SRSaturation::processPirkleModified(double &in1, double &in2) {
			if (mAmountNormalized > .001) {
				double mAmountModified = pow(mAmountNormalized, 3.) * (1. + 0.5 * (in1 - in1PrevSample + in2 - in2PrevSample) * (1. / mDriveNormalized) * mSkewNormalized);
				in1 = (in1 >= 0)
					? atan(mAmountModified * in1) / atan(mAmountModified)
					: atan(mAmountModified * in1) / atan(mAmountModified);

				in2 = (in2 >= 0.)
					? atan(mAmountModified * in2) / atan(mAmountModified)
					: atan(mAmountModified * in2) / atan(mAmountModified);
			}
		}

		inline void SRSaturation::processRectHalf(double &in1, double &in2) {
			in1 = (in1 < 0.) ? 0. : in1;
			in2 = (in2 < 0.) ? 0. : in2;
		}

		inline void SRSaturation::processRectFull(double &in1, double &in2) {
			in1 = fabs(in1);
			in2 = fabs(in2);
		}
	}
}

#endif // SRSaturation_h