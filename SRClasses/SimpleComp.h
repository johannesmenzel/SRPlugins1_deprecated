/*
 *	Simple Compressor (header)
 *
 *  File		: SimpleComp.h
 *	Library		: SimpleSource
 *  Version		: 1.12
 *  Class		: SimpleComp, SimpleCompRms
 *
 *	© 2006, ChunkWare Music Software, OPEN-SOURCE
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a
 *	copy of this software and associated documentation files (the "Software"),
 *	to deal in the Software without restriction, including without limitation
 *	the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *	and/or sell copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *	DEALINGS IN THE SOFTWARE.
 */


#ifndef __SIMPLE_COMP_H__
#define __SIMPLE_COMP_H__

#include "SimpleHeader.h"		// common header
#include "SimpleEnvelope.h"		// for base class
#include "SimpleGain.h"			// for gain functions
#include "SRFilters.h"			// for sidechain filter

namespace SRDynamics
{
	//-------------------------------------------------------------
	// simple compressor
	//-------------------------------------------------------------
	class SRCompressor : public AttRelEnvelope
	{
	public:
		SRCompressor();
		virtual ~SRCompressor() {}



		// parameters
		virtual void setCompressor(double dB, double ratio, double attackMs, double releaseMs, double sidechainFc, double kneeDb, double samplerate);
		virtual void setThresh( double dB );
		virtual void setRatio( double dB );
		virtual void setKnee(double kneeDb);
		virtual void setSidechainFilter(double sidechainFC);
		virtual void setSidechainFilterFreq(double sidechainFc);

		virtual double getThresh( void ) const { return threshdB_; }
		virtual double getRatio( void ) const { return ratio_; }
		virtual double getGrLin(void) { return grLin; } // Returns GR Amp
		virtual double getGrDb(void) { return grDb; } // Returns GR dB

		// runtime
		virtual void initRuntime( void );			// call before runtime (in resume())
		void process( double &in1, double &in2 );	// compressor runtime process
		void process( double &in1, double &in2, double keyLinked );	// with stereo-linked key in

	private:

		// transfer function
		double threshdB_;		// threshold (dB)
		double ratio_;			// ratio (compression: < 1 ; expansion: > 1)
		double grLin;
		double grDb;
		double mSidechainFc;
		double kneeDb_;
		double maxGr;

		// runtime variables
		double currentOvershootDb;			// over-threshold envelope (dB)

		SRFiltersTwoPole fSidechainFilter1, fSidechainFilter2;

	};	// end SimpleComp class

	//-------------------------------------------------------------
	// simple compressor with RMS detection
	//-------------------------------------------------------------
	class SRCompressorRMS : public SRCompressor
	{
	public:
		SRCompressorRMS();
		virtual ~SRCompressorRMS() {}

		// sample rate
		virtual void setSampleRate( double sampleRate );

		// RMS window
		virtual void setWindow( double ms );
		virtual double getWindow( void ) const { return ave_.getTc(); }

		// runtime process
		virtual void initRuntime( void );			// call before runtime (in resume())
		void process( double &in1, double &in2 );	// compressor runtime process

	private:

		EnvelopeDetector ave_;	// averager
		double aveOfSqrs_;		// average of squares

	};	// end SimpleCompRms class


	inline void SRCompressor::process(double &in1, double &in2)
	{
		// create sidechain
		double rect1 = in1;
		double rect2 = in2;
		if (mSidechainFc > 16. / getSampleRate()) {
			rect1 = fSidechainFilter1.process(rect1);
			rect2 = fSidechainFilter2.process(rect2);
		}

		rect1 = fabs(rect1);	// rectify input
		rect2 = fabs(rect2);


		/* if desired, one could use another EnvelopeDetector to smooth
		* the rectified signal.
		*/

		double link = std::max(rect1, rect2);	// link channels with greater of 2

		process(in1, in2, link);	// rest of process
	}


	//-------------------------------------------------------------
	inline void SRCompressor::process(double &in1, double &in2, double keyLinked)
	{
		keyLinked = fabs(keyLinked);		// rectify (just in case)

											// convert key to dB
		keyLinked += DC_OFFSET;				// add DC offset to avoid log( 0 )
		double keydB = lin2dB(keyLinked);	// convert linear -> dB

											// threshold
		double sampleOvershootDb = keydB - threshdB_;	// delta over threshold
		//if (overdB < 0.0)
		//	overdB = 0.0;

		// attack/release

		sampleOvershootDb += DC_OFFSET;					// add DC offset to avoid denormal
		AttRelEnvelope::run(sampleOvershootDb, currentOvershootDb);	// run attack/release envelope

												/* REGARDING THE DC OFFSET: In this case, since the offset is added before
												* the attack/release processes, the envelope will never fall below the offset,
												* thereby avoiding denormals. However, to prevent the offset from causing
												* constant gain reduction, we must subtract it from the envelope, yielding
												* a minimum value of 0dB.
												*/

												// transfer function
		sampleOvershootDb = currentOvershootDb - DC_OFFSET; // subtract DC offset
		//grDb = overdB * (ratio_ - 1.0);	// gain reduction (dB)

		double grRaw;

		if (sampleOvershootDb > kneeDb_ * 0.5) {
			grRaw = (ratio_ - 1.) * (sampleOvershootDb);											// For linear gain reduction above knee range
		}
		else if (fabs(sampleOvershootDb) <= kneeDb_ * 0.5) {
			grRaw = ((ratio_ - 1.) * pow(sampleOvershootDb + kneeDb_ * 0.5, 2.)) / (2. * kneeDb_); // For smoothed gain reduction within knee range
		}
		else {
			grRaw = 0.;																			// For no gain reduction below knee range
		}

		//double grlimit = maxGr / grRaw;

		double grlimit = grRaw / (maxGr*0.5);
		double grlimitsqrt = pow(grlimit, 0.8);

		grRaw = (1-grlimitsqrt < 0.) ? grRaw + ((1-grlimitsqrt) * (grRaw - (maxGr*0.5)))/grlimit : grRaw;

		grDb = grRaw;
		grRaw = dB2lin(grRaw);
		grLin = grRaw;
												// output gain
		in1 *= grRaw;	// apply gain reduction to input
		in2 *= grRaw;
	}


	//-------------------------------------------------------------
	inline void SRCompressorRMS::process(double &in1, double &in2)
	{
		// create sidechain

		double inSq1 = in1 * in1;	// square input
		double inSq2 = in2 * in2;

		double sum = inSq1 + inSq2;			// power summing
		sum += DC_OFFSET;					// DC offset, to prevent denormal
		ave_.run(sum, aveOfSqrs_);		// average of squares
		double rms = sqrt(aveOfSqrs_);	// rms (sort of ...)

										/* REGARDING THE RMS AVERAGER: Ok, so this isn't a REAL RMS
										* calculation. A true RMS is an FIR moving average. This
										* approximation is a 1-pole IIR. Nonetheless, in practice,
										* and in the interest of simplicity, this method will suffice,
										* giving comparable results.
										*/

		SRCompressor::process(in1, in2, rms);	// rest of process
	}
}	// end namespace

#endif	// end __SIMPLE_COMP_H__
