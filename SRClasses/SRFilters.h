//
//  Biquad.h
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/25/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//


#ifndef SRFilters_h
#define SRFilters_h
namespace SRPlugins {

	namespace SRFilters {

		// TWO POLE FILTERS

		enum {
			biquad_lowpass = 0,
			biquad_highpass,
			biquad_bandpass,
			biquad_notch,
			biquad_peak,
			biquad_peak_ncq,
			biquad_lowshelf,
			biquad_highshelf,
			iir_linkwitz_highpass,
			iir_linkwitz_lowpass
		};

		class SRFiltersTwoPole {
		public:
			SRFiltersTwoPole();
			SRFiltersTwoPole(int type, double Fc, double Q, double peakGainDB, double samplerate);
			~SRFiltersTwoPole();
			void setType(int type);
			void setQ(double Q);
			void setFc(double Fc);
			void setPeakGain(double peakGainDB);
			void setFilter(int type, double Fc, double Q, double peakGain, double samplerate);
			double process(double in);
			double getFreqResp(double plotFc);

		protected:
			void calcBiquad(void);

			int type;
			double a0, a1, a2, b1, b2;
			double Fc, Q, peakGain;
			double z1, z2;
			double samplerate;
			double plotFC;
		};

		inline double SRFiltersTwoPole::process(double in) {
			double out = in * a0 + z1;
			z1 = in * a1 + z2 - b1 * out;
			z2 = in * a2 - b2 * out;
			return out;
		}


		// ONE POLE FILTERS

		class SRFiltersOnePole {
		public:
			SRFiltersOnePole();
			SRFiltersOnePole(int type, double Fc, double samplerate);
			void setFilter(int type, double Fc, double samplerate);
			void setType(int type);
			~SRFiltersOnePole();
			void setFc(double Fc);
			void calcOnePole(void);
			double process(double in);

		protected:
			int type;
			double a0, b1, z1;
			double Fc;
			double samplerate;
			//double plotFC;
		};

		inline SRFiltersOnePole::~SRFiltersOnePole()
		{
		}

		inline double SRFiltersOnePole::process(double in) {
			//	return z1 = in * a0 + z1 * b1;
			z1 = in * a0 + z1 * b1;
			in -= z1;
			return in;
		}

	}
}
// end namespace SRFilters

#endif // SRFilters_h