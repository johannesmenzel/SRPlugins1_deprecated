/*
*	copyright 2018, Johannes Menzel, MIT
*
*	This is an ongoing modification of the code provided in the Simple Source
*	library by Bojan Markovic. Licence disclaimer is merged here, since the
*	classes and functions were merged to one .h and .cpp source file each.
*	See the original disclaimer below:
*
*	copyright 2006, ChunkWare Music Software, OPEN-SOURCE
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

#pragma once

#ifndef __SRDYNAMICS_H__
#define __SRDYNAMICS_H__

#if _MSC_VER > 1000				// MS Visual Studio
#define INLINE __forceinline	// forces inline
#define NOMINMAX				// for standard library min(), max()
#define _USE_MATH_DEFINES		// for math constants
#else							// other IDE's
#define INLINE inline
#endif

#include <algorithm>			// for min(), max()
#include <cassert>				// for assert()
#include <cmath>
#include <vector>
#include "SRFilters.h"
#include "SRHelpers.h"


namespace SRPlugins {
	namespace SRDynamics {


		//-------------------------------------------------------------
		// DC offset (to prevent denormal)
		//-------------------------------------------------------------
														// USE:
														// 1. init envelope state to DC_OFFSET before processing
														// 2. add to input before envelope runtime function
		static const double DC_OFFSET = 1.0E-25;


		//-------------------------------------------------------------
		// ENVELOPE DETECTOR Class
		//-------------------------------------------------------------
		class EnvelopeDetector
		{
		public:
			EnvelopeDetector(
				double ms = 1.0
				, double sampleRate = 44100.0
			);
			virtual ~EnvelopeDetector() {}

			// time constant
			virtual void   setTc(double ms);
			virtual double getTc(void) const { return mTimeConstantMs; }

			// sample rate
			virtual void   setSampleRate(double sampleRate);
			virtual double getSampleRate(void) const { return mSampleRate; }

			// runtime function
			INLINE void run(double in, double &state) {
				state = in + mRuntimeCoeff * (state - in);
			}

		protected:

			double mSampleRate;		// sample rate
			double mTimeConstantMs;				// time constant in ms
			double mRuntimeCoeff;			// runtime coefficient
			virtual void setCoef(void);	// coef calculation

		};
		//-------------------------------------------------------------
		// End Envelope Detector Class





		//-------------------------------------------------------------
		// ATTACK/RELEASE ENVELOPE Class
		//-------------------------------------------------------------
		class AttRelEnvelope
		{
		public:
			AttRelEnvelope(
				double mAttackMs = 10.0
				, double mReleaseMs = 100.0
				, double sampleRate = 44100.0
			);
			virtual ~AttRelEnvelope() {}

			// attack time constant
			virtual void   setAttack(double ms);
			virtual double getAttack(void) const { return mEnvelopeDetectorAttack.getTc(); }

			// release time constant
			virtual void   setRelease(double ms);
			virtual double getRelease(void) const { return mEnvelopeDetectorRelease.getTc(); }

			// sample rate dependencies
			virtual void   setSampleRate(double sampleRate);
			virtual double getSampleRate(void) const { return mEnvelopeDetectorAttack.getSampleRate(); }

			// runtime function
			INLINE void run(double in, double &state) {

				/* assumes that:
				* positive delta = attack
				* negative delta = release
				* good for linear & log values
				*/

				if (in > state)
					mEnvelopeDetectorAttack.run(in, state);	// attack
				else
					mEnvelopeDetectorRelease.run(in, state);	// release
			}

		private:

			EnvelopeDetector mEnvelopeDetectorAttack;
			EnvelopeDetector mEnvelopeDetectorRelease;

		};
		//-------------------------------------------------------------
		// End Attack/Release Envelope Class






		//-------------------------------------------------------------
		// COMPRESSOR Class
		//-------------------------------------------------------------
		class SRCompressor : public AttRelEnvelope
		{
		public:
			SRCompressor();
			virtual ~SRCompressor() {}

			// parameters
			virtual void setCompressor(double dB, double ratio, double attackMs, double releaseMs, double sidechainFc, double kneeDb, double samplerate);
			virtual void setThresh(double dB);
			virtual void setRatio(double dB);
			virtual void setKnee(double kneeDb);
			virtual void initSidechainFilter(double sidechainFC);
			virtual void setSidechainFilterFreq(double sidechainFc);

			virtual double getThresh(void) const { return mThreshDb; }
			virtual double getRatio(void) const { return mRatio; }
			virtual double getGrLin(void) { return mGrLin; } // Returns GR Amp
			virtual double getGrDb(void) { return mGrDb; } // Returns GR dB

														  // runtime
			virtual void initRuntime(void);
			// call before runtime (in resume())
			void process(double &in1, double &in2); // compressor runtime process if internal sidechain 
			void process(double &in1, double &in2, double &extSC1, double &extSC2); // if eternal sidechain
			void process(double &in1, double &in2, double sidechain);	// with stereo-linked key in

		private:

			// transfer function
			double mThreshDb;		// threshold (dB)
			double mRatio;			// ratio (compression: < 1 ; expansion: > 1)
			double mGrLin;
			double mGrDb;
			double mSidechainFc;
			double mKneeWidthDb;
			double mMaxGr;

			// runtime variables
			double currentOvershootDb;			// over-threshold envelope (dB)

			SRFilters::SRFiltersTwoPole fSidechainFilter1, fSidechainFilter2;

		};
		//-------------------------------------------------------------
		// End Compressor Class





		//-------------------------------------------------------------
		// RMS COMPRESSOR Class
		//-------------------------------------------------------------
		class SRCompressorRMS : public SRCompressor
		{
		public:
			SRCompressorRMS();
			virtual ~SRCompressorRMS() {}

			// sample rate
			virtual void setSampleRate(double sampleRate);

			// RMS window
			virtual void setWindow(double ms);
			virtual double getWindow(void) const { return mEnvelopeDetectorAverager.getTc(); }

			// runtime process
			virtual void initRuntime(void);
			// call before runtime (in resume())
			void process(double &in1, double &in2, double &extSC1, double &extSC2);
			void process(double &in1, double &in2);	// compressor runtime process

		private:

			EnvelopeDetector mEnvelopeDetectorAverager;	// averager
			double mAverageOfSquares;		// average of squares

		};
		//-------------------------------------------------------------
		// end RMS Compressor Class





		//-------------------------------------------------------------
		// COMPRESSOR Inline Functions
		//-------------------------------------------------------------

		// Inline Compressor Sidechain
		//-------------------------------------------------------------

		inline void SRCompressor::process(double &in1, double &in2, double &extSC1, double &extSC2) {
			double rectifiedInput1 = extSC1;
			double rectifiedInput2 = extSC2;
			// create sidechain
			if (mSidechainFc > 16. / getSampleRate()) {
				rectifiedInput1 = fSidechainFilter1.process(rectifiedInput1);
				rectifiedInput2 = fSidechainFilter2.process(rectifiedInput2);
			}

			rectifiedInput1 = fabs(rectifiedInput1);	// rectify input
			rectifiedInput2 = fabs(rectifiedInput2);


			/* if desired, one could use another EnvelopeDetector to smooth
			* the rectified signal.
			*/

			double rectifiedInputMaxed = std::max(rectifiedInput1, rectifiedInput2);	// link channels with greater of 2

			process(in1, in2, rectifiedInputMaxed);	// rest of process
		}

		inline void SRCompressor::process(double &in1, double &in2) {
			double rectifiedInput1 = in1;
			double rectifiedInput2 = in2;
			// create sidechain
			if (mSidechainFc > 16. / getSampleRate()) {
				rectifiedInput1 = fSidechainFilter1.process(rectifiedInput1);
				rectifiedInput2 = fSidechainFilter2.process(rectifiedInput2);
			}

			rectifiedInput1 = fabs(rectifiedInput1);	// rectify input
			rectifiedInput2 = fabs(rectifiedInput2);


			/* if desired, one could use another EnvelopeDetector to smooth
			* the rectified signal.
			*/

			double rectifiedInputMaxed = std::max(rectifiedInput1, rectifiedInput2);	// link channels with greater of 2

			process(in1, in2, rectifiedInputMaxed);	// rest of process
		}


		// Inline RMS Compressor Sidechain
		//-------------------------------------------------------------

		inline void SRCompressorRMS::process(double &in1, double &in2, double &extSC1, double &extSC2) {
			// create sidechain

			double squaredInput1 = extSC1 * extSC1;	// square input
			double squaredInput2 = extSC2 * extSC2;

			double summedSquaredInput = squaredInput1 + squaredInput2;			// power summing
			summedSquaredInput += DC_OFFSET;					// DC offset, to prevent denormal
			mEnvelopeDetectorAverager.run(summedSquaredInput, mAverageOfSquares);		// average of squares
			double sidechainRms = sqrt(mAverageOfSquares);	// sidechainRms (sort of ...)

															/* REGARDING THE RMS AVERAGER: Ok, so this isn't a REAL RMS
															* calculation. A true RMS is an FIR moving average. This
															* approximation is a 1-pole IIR. Nonetheless, in practice,
															* and in the interest of simplicity, this method will suffice,
															* giving comparable results.
															*/

			SRCompressor::process(in1, in2, sidechainRms);	// rest of process
		}

		inline void SRCompressorRMS::process(double &in1, double &in2) {
			// create sidechain

			double squaredInput1 = in1 * in1;	// square input
			double squaredInput2 = in2 * in2;

			double summedSquaredInput = squaredInput1 + squaredInput2;			// power summing
			summedSquaredInput += DC_OFFSET;					// DC offset, to prevent denormal
			mEnvelopeDetectorAverager.run(summedSquaredInput, mAverageOfSquares);		// average of squares
			double sidechainRms = sqrt(mAverageOfSquares);	// sidechainRms (sort of ...)

															/* REGARDING THE RMS AVERAGER: Ok, so this isn't a REAL RMS
															* calculation. A true RMS is an FIR moving average. This
															* approximation is a 1-pole IIR. Nonetheless, in practice,
															* and in the interest of simplicity, this method will suffice,
															* giving comparable results.
															*/

			SRCompressor::process(in1, in2, sidechainRms);	// rest of process
		}

		// Inline Compressors Process
		//-------------------------------------------------------------
		inline void SRCompressor::process(double &in1, double &in2, double sidechain) {
			sidechain = fabs(sidechain);		// rectify (just in case)

												// convert key to dB
			sidechain += DC_OFFSET;				// add DC offset to avoid log( 0 )
			double sidechainDb = SRPlugins::SRHelpers::AmpToDB(sidechain);	// convert linear -> dB

												// threshold
			double sampleOvershootDb = sidechainDb - mThreshDb;	// delta over threshold
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

			if (sampleOvershootDb > mKneeWidthDb * 0.5) {
				grRaw = (mRatio - 1.) * (sampleOvershootDb);											// For linear gain reduction above knee range
			}
			else if (fabs(sampleOvershootDb) <= mKneeWidthDb * 0.5) {
				grRaw = ((mRatio - 1.) * pow(sampleOvershootDb + mKneeWidthDb * 0.5, 2.)) / (2. * mKneeWidthDb); // For smoothed gain reduction within knee range
			}
			else {
				grRaw = 0.;																			// For no gain reduction below knee range
			}

			//double grlimit = mMaxGr / grRaw;

			double grlimit = grRaw / (mMaxGr * 0.5);
			double grlimitsqrt = pow(grlimit, 0.8);

			grRaw = (1 - grlimitsqrt < 0.) ? grRaw + ((1 - grlimitsqrt) * (grRaw - (mMaxGr*0.5))) / grlimit : grRaw;

			mGrDb = grRaw;
			grRaw = SRPlugins::SRHelpers::DBToAmp(grRaw);
			mGrLin = grRaw;
			// output gain
			in1 *= grRaw;	// apply gain reduction to input
			in2 *= grRaw;
		}
		//-------------------------------------------------------------
		// End Compressor Inline Functions





		//-------------------------------------------------------------
		// LIMITER Class
		//-------------------------------------------------------------
		class SRLimiter
		{
		public:
			SRLimiter();
			virtual ~SRLimiter() {}

			// parameters
			virtual void setThresh(double dB);
			virtual void setAttack(double ms);
			virtual void setRelease(double ms);

			virtual double getThresh(void)  const { return mThreshDb; }
			virtual double getAttack(void)  const { return mEnvelopeDetectorAttack.getTc(); }
			virtual double getRelease(void) const { return mEnvelopeDetectorRelease.getTc(); }

			// latency
			virtual const unsigned int getLatency(void) const { return mPeakHoldSamples; }

			// sample rate dependencies
			virtual void   setSampleRate(double sampleRate);
			virtual double getSampleRate(void) { return mEnvelopeDetectorAttack.getSampleRate(); }

			// runtime
			virtual void initRuntime(void);			// call before runtime (in resume())
			void process(double &in1, double &in2);	// limiter runtime process

		protected:

			// class for faster attack/release
			class FastEnvelope : public EnvelopeDetector
			{
			public:
				FastEnvelope(double ms = 1.0, double sampleRate = 44100.0)
					: EnvelopeDetector(ms, sampleRate)
				{}
				virtual ~FastEnvelope() {}

			protected:
				// override setCoef() - coefficient calculation
				virtual void setCoef(void);
			};

		private:

			// transfer function
			double mThreshDb;	// threshold (dB)
			double mThreshLin;		// threshold (linear)

								// max peak
			unsigned int mPeakHoldSamples;		// peak hold (samples)
			unsigned int mPeakHoldTimer;	// peak hold timer
			double mMaxPeak;			// max peak

										// attack/release envelope
			FastEnvelope mEnvelopeDetectorAttack;			// attack
			FastEnvelope mEnvelopeDetectorRelease;			// release
			double currentOvershootLin;				// over-threshold envelope (linear)

										// buffer
										// BUFFER_SIZE default can handle up to ~10ms at 96kHz
										// change this if you require more
			static const int BUFFER_SIZE = 1024;	// buffer size (always a power of 2!)
			unsigned int mBufferMask;						// buffer mask
			unsigned int mCursor;						// cursor
			std::vector< double > mOutputBuffer[2];	// output buffer

		};
		//-------------------------------------------------------------
		// End Limiter Class





		//-------------------------------------------------------------
		// LIMITER Inline Functions
		//-------------------------------------------------------------
		INLINE void SRLimiter::process(double &in1, double &in2)
		{
			// create sidechain

			double rectifiedInput1 = fabs(in1);	// rectify input
			double rectifiedInput2 = fabs(in2);

			double keyLink = std::max(rectifiedInput1, rectifiedInput2);	// link channels with greater of 2

														// threshold
														// we always want to feed the sidechain AT LEATS the threshold value
			if (keyLink < mThreshLin)
				keyLink = mThreshLin;

			// test:
			// a) whether peak timer has "expired"
			// b) whether new peak is greater than previous max peak
			if ((++mPeakHoldTimer >= mPeakHoldSamples) || (keyLink > mMaxPeak)) {
				// if either condition is met:
				mPeakHoldTimer = 0;		// reset peak timer
				mMaxPeak = keyLink;	// assign new peak to max peak
			}

			/* REGARDING THE MAX PEAK: This method assumes that the only important
			* sample in a look-ahead buffer would be the highest peak. As such,
			* instead of storing all samples in a look-ahead buffer, it only stores
			* the max peak, and compares all incoming samples to that one.
			* The max peak has a hold time equal to what the look-ahead buffer
			* would have been, which is tracked by a timer (counter). When this
			* timer expires, the sample would have exited from the buffer. Therefore,
			* a new sample must be assigned to the max peak. We assume that the next
			* highest sample in our theoretical buffer is the current input sample.
			* In reality, we know this is probably NOT the case, and that there has
			* been another sample, slightly lower than the one before it, that has
			* passed the input. If we do not account for this possibility, our gain
			* reduction could be insufficient, resulting in an "over" at the output.
			* To remedy this, we simply apply a suitably long release stage in the
			* envelope follower.
			*/

			// attack/release
			if (mMaxPeak > currentOvershootLin)
				mEnvelopeDetectorAttack.run(mMaxPeak, currentOvershootLin);		// run attack phase
			else
				mEnvelopeDetectorRelease.run(mMaxPeak, currentOvershootLin);		// run release phase

												/* REGARDING THE ATTACK: This limiter achieves "look-ahead" detection
												* by allowing the envelope follower to attack the max peak, which is
												* held for the duration of the attack phase -- unless a new, higher
												* peak is detected. The output signal is buffered so that the gain
												* reduction is applied in advance of the "offending" sample.
												*/

												/* NOTE: a DC offset is not necessary for the envelope follower,
												* as neither the max peak nor envelope should fall below the
												* threshold (which is assumed to be around 1.0 linear).
												*/

												// gain reduction
			double grRaw = mThreshLin / currentOvershootLin;

			// unload current buffer index
			// ( mCursor - delay ) & mBufferMask gets sample from [delay] samples ago
			// mBufferMask variable wraps index
			unsigned int delayIndex = (mCursor - mPeakHoldSamples) & mBufferMask;
			double delay1 = mOutputBuffer[0][delayIndex];
			double delay2 = mOutputBuffer[1][delayIndex];

			// load current buffer index and advance current index
			// mBufferMask wraps mCursor index
			mOutputBuffer[0][mCursor] = in1;
			mOutputBuffer[1][mCursor] = in2;
			++mCursor &= mBufferMask;

			// output gain
			in1 = delay1 * grRaw;	// apply gain reduction to input
			in2 = delay2 * grRaw;

			/* REGARDING THE GAIN REDUCTION: Due to the logarithmic nature
			* of the attack phase, the sidechain will never achieve "full"
			* attack. (Actually, it is only guaranteed to achieve 99% of
			* the input value over the given time constant.) As such, the
			* limiter cannot achieve "brick-wall" limiting. There are 2
			* workarounds:
			*
			* 1) Set the threshold slightly lower than the desired threshold.
			*		i.e. 0.0dB -> -0.1dB or even -0.5dB
			*
			* 2) Clip the output at the threshold, as such:
			*
			*		if ( in1 > mThreshLin )		in1 = mThreshLin;
			*		else if ( in1 < -mThreshLin )	in1 = -mThreshLin;
			*
			*		if ( in2 > mThreshLin )		in2 = mThreshLin;
			*		else if ( in2 < -mThreshLin )	in2 = -mThreshLin;
			*
			*		(... or replace with your favorite branchless clipper ...)
			*/
		}
		//-------------------------------------------------------------
		// End Limiter Inline Functions





		//-------------------------------------------------------------
		// GATE Class
		//-------------------------------------------------------------
		class SRGate : public AttRelEnvelope
		{
		public:
			SRGate();
			virtual ~SRGate() {}

			// parameters
			virtual void   setThresh(double dB);
			virtual double getThresh(void) const { return mThreshDb; }

			// runtime
			virtual void initRuntime(void);			// call before runtime (in resume())
			void process(double &in1, double &in2);	// gate runtime process
			void process(double &in1, double &in2, double keyLinked);	// with stereo-linked key in

		private:

			// transfer function
			double mThreshDb;	// threshold (dB)
			double mThreshLin;		// threshold (linear)

								// runtime variables
			double currentOvershootLin;		// over-threshold envelope (linear)

		};
		//-------------------------------------------------------------
		// End Gate Class





		//-------------------------------------------------------------
		// GATE RMS Class
		//-------------------------------------------------------------
		class SRGateRms : public SRGate
		{
		public:
			SRGateRms();
			virtual ~SRGateRms() {}

			// sample rate
			virtual void setSampleRate(double sampleRate);

			// RMS window
			virtual void setWindow(double ms);
			virtual double getWindow(void) const { return mEnvelopeDetectorAverager.getTc(); }

			// runtime process
			virtual void initRuntime(void);			// call before runtime (in resume())
			void process(double &in1, double &in2);	// gate runtime process

		private:

			EnvelopeDetector mEnvelopeDetectorAverager;	// averager
			double mAverageOfSquares;		// average of squares

		};
		//-------------------------------------------------------------
		// end SimpleGateRms class





		//-------------------------------------------------------------
		// GATE Inline Functions
		//-------------------------------------------------------------

		// Inline Gate Sidechain
		//-------------------------------------------------------------
		INLINE void SRGate::process(double &in1, double &in2)
		{
			// create sidechain

			double rectifiedInput1 = fabs(in1);	// rectify input
			double rectifiedInput2 = fabs(in2);

			/* if desired, one could use another EnvelopeDetector to smooth
			* the rectified signal.
			*/

			double rectifiedInputMaxed = std::max(rectifiedInput1, rectifiedInput2);	// link channels with greater of 2

			process(in1, in2, rectifiedInputMaxed);	// rest of process
		}

		// Inline RMS Gate Sidechain
		//-------------------------------------------------------------
		INLINE void SRGateRms::process(double &in1, double &in2)
		{
			// create sidechain

			double squaredInput1 = in1 * in1;	// square input
			double squaredInput2 = in2 * in2;

			double summedSquaredInput = squaredInput1 + squaredInput2;			// power summing
			summedSquaredInput += DC_OFFSET;					// DC offset, to prevent denormal
			mEnvelopeDetectorAverager.run(summedSquaredInput, mAverageOfSquares);		// average of squares
			double sidechainRms = sqrt(mAverageOfSquares);	// sidechainRms (sort of ...)

											/* REGARDING THE RMS AVERAGER: Ok, so this isn't a REAL RMS
											* calculation. A true RMS is an FIR moving average. This
											* approximation is a 1-pole IIR. Nonetheless, in practice,
											* and in the interest of simplicity, this method will suffice,
											* giving comparable results.
											*/

			SRGate::process(in1, in2, sidechainRms);	// rest of process
		}

		// Inline Gates Process
		//-------------------------------------------------------------
		INLINE void SRGate::process(double &in1, double &in2, double sidechain)
		{
			sidechain = fabs(sidechain);	// rectify (just in case)

											// threshold
											// key over threshold ( 0.0 or 1.0 )
			double gateGainApply = double(sidechain > mThreshLin);

			// attack/release
			gateGainApply += DC_OFFSET;					// add DC offset to avoid denormal
			AttRelEnvelope::run(gateGainApply, currentOvershootLin);	// run attack/release
			gateGainApply = currentOvershootLin - DC_OFFSET;			// subtract DC offset

												/* REGARDING THE DC OFFSET: In this case, since the offset is added before
												* the attack/release processes, the envelope will never fall below the offset,
												* thereby avoiding denormals. However, to prevent the offset from causing
												* constant gain reduction, we must subtract it from the envelope, yielding
												* a minimum value of 0dB.
												*/

												// output gain
			in1 *= gateGainApply;	// apply gain reduction to input
			in2 *= gateGainApply;
		}
		//-------------------------------------------------------------
		// End Gate Inline Functions


		//-------------------------------------------------------------
		// DEESSER Class
		//-------------------------------------------------------------
		class SRDeesser : public AttRelEnvelope
		{
		public:
			SRDeesser();
			virtual ~SRDeesser() {}

			// parameters
			virtual void setDeesser(double threshDb, double ratio, double attackMs, double releaseMs, double freqNormalized, double q, double kneeDb, double samplerate);
			virtual void setThresh(double dB);
			virtual void setRatio(double dB);
			virtual void setKnee(double kneeDb);
			virtual void initFilter(double freq, double q);
			virtual void setFrequency(double freq);
			virtual void setQ(double q);

			virtual double getThresh(void) const { return mThreshDb; }
			virtual double getRatio(void) const { return mRatio; }
			virtual double getGrLin(void) { return mGrLin; } // Returns GR Amp
			virtual double getGrDb(void) { return mGrDb; } // Returns GR dB

														   // runtime
			virtual void initRuntime(void);
			// call before runtime (in resume())
			void process(double &in1, double &in2); // compressor runtime process if internal sidechain 
			void process(double &in1, double &in2, double sidechain);	// with stereo-linked key in

		private:

			// transfer function
			double mThreshDb;		// threshold (dB)
			double mRatio;			// ratio (compression: < 1 ; expansion: > 1)
			double mFilterFreq;
			double mFilterQ;
			double mFilterGain;
			double mGrLin;
			double mGrDb;
			double mKneeWidthDb;

			// runtime variables
			double currentOvershootDb;			// over-threshold envelope (dB)

			SRFilters::SRFiltersTwoPole fSidechainBandpass1, fSidechainBandpass2, fDynamicEqFilter1, fDynamicEqFilter2;

		};
		//-------------------------------------------------------------
		// End SRDeesser Class



		inline void SRDeesser::process(double &in1, double &in2) {
			double rectifiedInput1 = in1;
			double rectifiedInput2 = in2;
			// create sidechain
			rectifiedInput1 = fSidechainBandpass1.process(rectifiedInput1);
			rectifiedInput2 = fSidechainBandpass2.process(rectifiedInput2);


			rectifiedInput1 = fabs(rectifiedInput1);	// rectify input
			rectifiedInput2 = fabs(rectifiedInput2);


			/* if desired, one could use another EnvelopeDetector to smooth
			* the rectified signal.
			*/

			double rectifiedInputMaxed = std::max(rectifiedInput1, rectifiedInput2);	// link channels with greater of 2

			process(in1, in2, rectifiedInputMaxed);	// rest of process
		}


		// Inline SRDeesser Process
		//-------------------------------------------------------------
		inline void SRDeesser::process(double &in1, double &in2, double sidechain) {
			sidechain = fabs(sidechain);		// rectify (just in case)

												// convert key to dB
			sidechain += DC_OFFSET;				// add DC offset to avoid log( 0 )
			double sidechainDb = SRPlugins::SRHelpers::AmpToDB(sidechain);	// convert linear -> dB

																			// threshold
			double sampleOvershootDb = sidechainDb - mThreshDb;	// delta over threshold
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

			if (sampleOvershootDb > mKneeWidthDb * 0.5) {
				grRaw = (mRatio - 1.) * (sampleOvershootDb);											// For linear gain reduction above knee range
			}
			else if (fabs(sampleOvershootDb) <= mKneeWidthDb * 0.5) {
				grRaw = ((mRatio - 1.) * pow(sampleOvershootDb + mKneeWidthDb * 0.5, 2.)) / (2. * mKneeWidthDb); // For smoothed gain reduction within knee range
			}
			else {
				grRaw = 0.;																			// For no gain reduction below knee range
			}


			mGrDb = grRaw;
			mGrLin = SRPlugins::SRHelpers::DBToAmp(grRaw);
			fDynamicEqFilter1.setPeakGain(grRaw);
			fDynamicEqFilter2.setPeakGain(grRaw);
			in1 = fDynamicEqFilter1.process(in1);
			in2 = fDynamicEqFilter2.process(in2);
		}
		//-------------------------------------------------------------
		// End SRDeesser Inline Functions





	}	// end namespace SRDynamics
}	// end namespace SRPlugins
#endif	// end __SRDYNAMICS_H__
