#include "SRGain.h"
//#include this and that
//#include <math.h>

namespace SRPlugins
{
	namespace SRGain
	{


		SRGain::SRGain()
		{
			mGainLin1 = mGainLin2 = 1.0;
			currentGain1 = currentGain2 = targetGain1 = targetGain2 = 1.0;
			stepGain1 = stepGain2 = 0.0;
			mRampNumSamples = 1.0;
			mBypassed = false;
		}
		SRGain::SRGain(double pGainLin1, double pGainLin2, double pRampNumSamples, bool pBypassed)
		{
			initGain(pGainLin1, pGainLin2, pRampNumSamples, pBypassed);
		}
		SRGain::~SRGain() {}

		void SRGain::initGainDb(double pGainDb1, double pGainDb2, double pRampNumSamples, bool pBypassed) {
			this->mGainLin1 = SRPlugins::SRHelpers::DBToAmp(pGainDb1);
			this->mGainLin2 = SRPlugins::SRHelpers::DBToAmp(pGainDb2);
			this->mRampNumSamples = pRampNumSamples;
			this->mBypassed = pBypassed;
			calcGain();
		}
		void SRGain::initGain(double pGainLin1, double pGainLin2, double pRampNumSamples, bool pBypassed)
		{
			this->mGainLin1 = pGainLin1;
			this->mGainLin2 = pGainLin2;
			this->mRampNumSamples = pRampNumSamples;
			this->mBypassed = pBypassed;
			calcGain();
		}
		void SRGain::setGain(double pGainLin)
		{
			this->mGainLin1 = pGainLin;
			this->mGainLin2 = pGainLin;
			calcGain();
		}
		void SRGain::setGain(double pGainLin1, double pGainLin2)
		{
			this->mGainLin1 = pGainLin1;
			this->mGainLin2 = pGainLin2;
			calcGain();
		}
		void SRGain::setGainDb(double pGainDb)
		{
			this->mGainLin1 = SRPlugins::SRHelpers::DBToAmp(pGainDb);
			this->mGainLin2 = SRPlugins::SRHelpers::DBToAmp(pGainDb);
			calcGain();
		}
		void SRGain::setGainDb(double pGainDb1, double pGainDb2)
		{
			this->mGainLin1 = SRPlugins::SRHelpers::DBToAmp(pGainDb1);
			this->mGainLin2 = SRPlugins::SRHelpers::DBToAmp(pGainDb2);
			calcGain();
		}
		void SRGain::setRamp(double pRampNumSamples)
		{
			this->mRampNumSamples = pRampNumSamples;
			mRampNumSamples = (mRampNumSamples < 1.0) ? 1.0 : mRampNumSamples;
			calcGain();
		}

		void SRGain::setBypassed(bool pBypassed)
		{
			this->mBypassed = pBypassed;
			calcGain();
		}

		void SRGain::calcGain(void)
		{
			targetGain1 = mGainLin1;
			targetGain2 = mGainLin2;
			stepGain1 = (targetGain1 - currentGain1) / mRampNumSamples;
			stepGain2 = (targetGain2 - currentGain2) / mRampNumSamples;
		}








		SRPan::SRPan()
		{
			// init member variable values so that non empty
			mType = typeLinear;
			mPanNormalized = 0.5;
			mLinearMiddlePosition = true;
			gain1 = 1.0;
			gain2 = 1.0;
		}

		// Constructor
		SRPan::SRPan(int pType, double pPanNormalized, bool pLinearMiddlePosition)
		{
			// set members on init
			initPan(pType, pPanNormalized, pLinearMiddlePosition);
			// init internal variables

		}

		// Destructor
		SRPan::~SRPan() {}

		void SRPan::setType(int pType)
		{
			this->mType = pType;
			calcPan();
		}

		void SRPan::setPanPosition(double pPanNormalized)
		{
			this->mPanNormalized = pPanNormalized;
			calcPan();
		}

		void SRPan::setLinearMiddlePosition(bool pLinearMiddlePosition)
		{
			this->mLinearMiddlePosition = pLinearMiddlePosition;
			calcPan();
		}

		void SRPan::initPan(int pType, double pPanNormalized, bool pLinearMiddlePosition)
		{
			this->mType = pType;
			this->mPanNormalized = pPanNormalized;
			this->mLinearMiddlePosition = pLinearMiddlePosition;
			panSmoother.initGain(gain1, gain2, 4096, false);
			calcPan();
		}

		// Internal calculations. Example on calculation depending on mType:
		void SRPan::calcPan(void)
		{
			switch (this->mType)
			{
			case typeLinear:
				if (mLinearMiddlePosition == true)
				{
					gain1 = (mPanNormalized > 0.5) ? (1 - mPanNormalized) * 2 : 1.0;
					gain2 = (mPanNormalized < 0.5) ? mPanNormalized * 2 : 1.0;
				}
				else
				{
					gain1 = (1 - mPanNormalized);
					gain2 = (mPanNormalized);
				}
				break;
			case typeSquareroot:
				if (mLinearMiddlePosition == true)
				{
					gain1 = (mPanNormalized > 0.5) ? sqrt(mPanNormalized) * sqrt(2.) : 1.0;
					gain2 = (mPanNormalized < 0.5) ? sqrt(1 - mPanNormalized) * sqrt(2) : 1.0;
				}
				else
				{
					gain1 = sqrt(1. - mPanNormalized);
					gain2 = sqrt(mPanNormalized);
				}
				break;
			case typeSinusodial:
				if (mLinearMiddlePosition == true)
				{
					gain1 = (mPanNormalized > 0.5) ? sin(2. * mPanNormalized * M_PI_2) : 1.0;
					gain2 = (mPanNormalized < 0.5) ? cos((1. - 2. * mPanNormalized) * M_PI_2) : 1.0;
				}
				else
				{
					gain1 = sin(mPanNormalized * M_PI_2);
					gain2 = cos(mPanNormalized * M_PI_2);
				}
				break;
			case typeTanh:
				gain1 = (mPanNormalized > 0.5) ? tanh(4. * (1 - mPanNormalized)) / tanh(2.) : 1.0; // you can scale it by replacing 4. and 2. by 2n and n (10. and 5.)
				gain2 = (mPanNormalized < 0.5) ? tanh(4. * mPanNormalized) / tanh(2.) : 1.0;
				break;
			default:
				break;
			}

			panSmoother.setGain(gain1, gain2);

			return;
		}

	} // namespace SRGain
} // namespace SRPlugins