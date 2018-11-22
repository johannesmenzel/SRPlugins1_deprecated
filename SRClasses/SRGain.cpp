#include "SRGain.h"
//#include this and that
//#include <math.h>

namespace SRPlugins
{
namespace SRGain
{

SRPan::SRPan()
{
	// init member variable values so that non empty
	mType = typeLinear;
	mPanNormalized = 0.5;
	mLinearMiddlePosition = true;
}

// Constructor
SRPan::SRPan(int pType, double pPanNormalized, bool pLinearMiddlePosition)
{
	// set members on init
	setSaturation(pType, pPanNormalized, pLinearMiddlePosition);
	// init internal variables
	gain1 = 1.0;
	gain2 = 1.0;
}

// Destructor
SRPan::~SRPan()
{
}

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

void SRPan::setPan(int pType, double pPanNormalized, bool pLinearMiddlePosition)
{
	this->mType = pType;
	this->mPanNormalized = pPanNormalized;
	this->mLinearMiddlePosition = pLinearMiddlePosition;
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
			gain1 = (mPanNormalized > 0.5) ? sin(2. * mPanNormalized * (pi / 2.)) : 1.0;
			gain2 = (mPanNormalized < 0.5) ? cos((1. - 2. * mPanNormalized) * (pi / 2.)) : 1.0;
		}
		else
		{
			gain1 = sin(mPanNormalized * (pi / 2));
			gain2 = cos(mPanNormalized * (pi / 2));
		}
		break;
	case typeTanh:
			gain1 = (mPanNormalized > 0.5) ? tanh(4. * mPanNormalized) / tanh(2.) : 1.0; // you can scale it by replacing 4. and 2. by 2n and n (10. and 5.)
			gain2 = (mPanNormalized < 0.5) ? tanh(4. * (1 - mPanNormalized)) / tanh(2.) : 1.0;
			break;
	default:
		break;
	}
	return;
}
} // namespace SRGain
} // namespace SRPlugins