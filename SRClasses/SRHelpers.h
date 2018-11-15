#ifndef SRHELPERS_H
#define SRHELPERS_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

namespace SRPlugins {
	namespace SRHelpers {

		static inline double DBToAmp(double dB) { return exp(0.11512925464970 * dB); }
		static inline double AmpToDB(double amp) { return 8.685889638065036553 * log(fabs(amp)); }
		
		inline double SetShapeCentered(double cMinValue, double cMaxValue, double cCenteredValue, double cControlPosition)
		{
			return log((cCenteredValue - cMinValue) / (cMaxValue - cMinValue)) / log(cControlPosition);
		}

		inline double fast_tanh(double x)
		{
			x = exp(x + x);
			return (x - 1) / (x + 1);
		}

		inline double calcAutoMakeup(double threshDb, double ratio, double referenceDb, double attackMs, double releaseMs) {
			//double threshLin = DBToAmp(threshDb);
			//double referenceLin = DBToAmp(referenceDb);
			//double multiplier = ((tanh(threshDb / referenceDb) + 1) / 2); // returns for x= inf; refDb; 0; -inf -> y= 1, .88, .5, 0
			//return 1. + ((1. - threshLin) * (1. - ratio));
			//mCompPeakAutoMakeup = (threshLin <= referenceLin) ? 1. + (1. - DBToAmp(mCompPeakThresh + 18)) * (1. - mCompPeakRatio) : 1.;
			//return 1. + (1. - DBToAmp(threshDb - referenceDb)) * (1. - ratio);

			//return 1. + ( (1. - threshLin) * (1. - ratio) * ((tanh(threshDb-referenceDb) + 1.) / 2.) );
			//return 1. + (1. / DBToAmp((ratio - 1.) * (referenceDb - threshDb))) * ((1. - threshLin) * (1. - ratio));
			//return 1.;

			return	1. + (1. / (DBToAmp(((ratio - 1.) * -threshDb) / 2)) - 1.) * sqrt(threshDb / referenceDb) * tanh(30. / attackMs) * tanh(releaseMs / 1000.);
			//return makeup;



		}



		//class SRHelpers
		//{
		//public:
		//	SRHelpers();
		//	~SRHelpers();
		//};

	}
}
// end namespaces

#endif