#include "SRSaturation.h"
//#include this and that
//#include <math.h>


namespace SRPlugins {
	namespace SRSaturation {



		SRSaturation::SRSaturation() {
			// declare variable values so that non empty
			mType = typeMusicDSP;
			// member variables
			mDriveNormalized = 1.;
			mAmountNormalized = 0.;
			mHarmonicsNormalized = .5;
			mPositive = true;
			mSkewNormalized = 0.;
			mWetNormalized = 1.;
		}

		// Constructor
		SRSaturation::SRSaturation(int pType, double pDriveDb, double pAmountNormalized, double pHarmonicsNormalized, bool pPositiveSide, double pSkewNormalized, double pWet) {
			// internal variables
			setSaturation(pType, pDriveDb, pAmountNormalized, pHarmonicsNormalized, pPositiveSide, pSkewNormalized, pWet);
			mAmount = 0.;
			in1PrevSample = 0.;
			in2PrevSample = 0.;
			in1Dry = 0.;
			in2Dry = 0.;
		}

		// Destructor
		SRSaturation::~SRSaturation() {
		}

		void SRSaturation::setType(int pType) {
			this->mType = pType;
			calcSaturation();
		}

		void SRSaturation::setDrive(double pDriveDb) {
			this->mDriveNormalized = SRPlugins::SRHelpers::DBToAmp(pDriveDb);
		}

		void SRSaturation::setAmount(double pAmountNormalized) {
			this->mAmountNormalized = pAmountNormalized;
			calcSaturation();
		}

		void SRSaturation::setHarmonics(double pHarmonicsNormalized) {
			this->mHarmonicsNormalized = pHarmonicsNormalized;
		}

		void SRSaturation::setPositive(bool pPositive) {
			this->mPositive = pPositive;
		}

		void SRSaturation::setSkew(double pSkewNormalized) {
			this->mSkewNormalized = pSkewNormalized;
		}

		void SRSaturation::setWet(double pWetNormalized) {
			this->mWetNormalized = pWetNormalized;
		}

		void SRSaturation::setSaturation(int pType, double pDriveDb, double pAmountNormalized, double pHarmonicsNormalized, bool pPositive, double pSkewNormalized, double pWetNormalized) {
			this->mType = pType;
			this->mDriveNormalized = SRPlugins::SRHelpers::DBToAmp(pDriveDb);
			this->mAmountNormalized = pAmountNormalized;
			this->mHarmonicsNormalized = pHarmonicsNormalized;
			this->mPositive = pPositive;
			this->mSkewNormalized = pSkewNormalized;
			this->mWetNormalized = pWetNormalized;
			calcSaturation();
		}


		// Internal calculations. Example on calculation depending on mType:
		void SRSaturation::calcSaturation(void) {
			switch (this->mType) {
			case typePirkle: break;
			case typePirkleModified: break;
			case typeZoelzer: break;
			case typeMusicDSP: this->mAmount = (1. - mAmountNormalized); break;
			case typeRectHalf: break;
			case typeRectFull: break;
			default: break;
			}
			return;
		}
	}
}