#include "SRDynamics.h"
namespace SRPlugins {

	namespace SRDynamics {

		//-------------------------------------------------------------
		// envelope detector
		//-------------------------------------------------------------
		EnvelopeDetector::EnvelopeDetector(double ms, double sampleRate)
		{
			assert(sampleRate > 0.0);
			assert(ms > 0.0);
			mSampleRate = sampleRate;
			mTimeConstantMs = ms;
			setCoef();
		}



		//-------------------------------------------------------------
		void EnvelopeDetector::setTc(double ms)
		{
			assert( ms > 0.0 );
			mTimeConstantMs = ms;
			setCoef();
		}

		//-------------------------------------------------------------
		void EnvelopeDetector::setSampleRate(double sampleRate)
		{
			assert(sampleRate > 0.0);
			mSampleRate = sampleRate;
			setCoef();
		}

		//-------------------------------------------------------------
		void EnvelopeDetector::setCoef(void)
		{
			mRuntimeCoeff = exp(-1000.0 / (mTimeConstantMs * mSampleRate));
		}

		//-------------------------------------------------------------
		// attack/release envelope
		//-------------------------------------------------------------
		AttRelEnvelope::AttRelEnvelope(double att_ms, double rel_ms, double sampleRate)
			: mEnvelopeDetectorAttack(att_ms, sampleRate)
			, mEnvelopeDetectorRelease(rel_ms, sampleRate)
		{
		}

		//-------------------------------------------------------------
		void AttRelEnvelope::setAttack(double ms)
		{
			mEnvelopeDetectorAttack.setTc(ms);
		}

		//-------------------------------------------------------------
		void AttRelEnvelope::setRelease(double ms)
		{
			mEnvelopeDetectorRelease.setTc(ms);
		}

		//-------------------------------------------------------------
		void AttRelEnvelope::setSampleRate(double sampleRate)
		{
			mEnvelopeDetectorAttack.setSampleRate(sampleRate);
			mEnvelopeDetectorRelease.setSampleRate(sampleRate);
		}



		//-------------------------------------------------------------
		// simple compressor
		//-------------------------------------------------------------
		SRCompressor::SRCompressor()
			: AttRelEnvelope(10.0, 100.0)
			, mThreshDb(0.0)
			, mRatio(1.0)
			, currentOvershootDb(DC_OFFSET)
			, mSidechainFc(0.0)
			, mGrLin(1.0)
			, mGrDb(0.0)
			, mKneeWidthDb(0.0)
			, mMaxGr(0.0)
		{
		}

		void SRCompressor::setCompressor(double dB, double ratio, double attackMs, double releaseMs, double sidechainFc, double kneeDb, double samplerate) {
			setThresh(dB);
			setRatio(ratio);
			setAttack(attackMs);
			setRelease(releaseMs);
			initSidechainFilter(sidechainFc);
			setSampleRate(samplerate);
			setKnee(kneeDb);
			fSidechainFilter1.setFilter(SRFilters::biquad_highpass, mSidechainFc, 0.7071, 0., samplerate);
			fSidechainFilter2.setFilter(SRFilters::biquad_highpass, mSidechainFc, 0.7071, 0., samplerate);
		}

		//-------------------------------------------------------------
		void SRCompressor::setThresh(double dB)
		{
			mThreshDb = dB;
		}

		//-------------------------------------------------------------
		void SRCompressor::setRatio(double ratio)
		{
			assert( ratio >= 0.0 );
			mRatio = ratio;
			mMaxGr = 73.4979484210802 - 88.939188010773 * (1 - exp(-1.75091102973106 * (1 / ratio)));
		}

		void SRCompressor::setKnee(double kneeDb)
		{
			mKneeWidthDb = kneeDb;
		}

		void SRCompressor::initSidechainFilter(double sidechainFc) {
			mSidechainFc = sidechainFc;
			fSidechainFilter1.setFilter(SRFilters::biquad_highpass, mSidechainFc, 0.7071, 0., getSampleRate());
			fSidechainFilter2.setFilter(SRFilters::biquad_highpass, mSidechainFc, 0.7071, 0., getSampleRate());
		}

		void SRCompressor::setSidechainFilterFreq(double sidechainFc)
		{
			mSidechainFc = sidechainFc;
			fSidechainFilter1.setFc(mSidechainFc);
			fSidechainFilter2.setFc(mSidechainFc);
		}

		//-------------------------------------------------------------
		void SRCompressor::initRuntime(void)
		{
			currentOvershootDb = DC_OFFSET;
		}

		//-------------------------------------------------------------
		// simple compressor with RMS detection
		//-------------------------------------------------------------
		SRCompressorRMS::SRCompressorRMS()
			: mEnvelopeDetectorAverager(5.0)
			, mAverageOfSquares(DC_OFFSET)
		{
		}

		//-------------------------------------------------------------
		void SRCompressorRMS::setSampleRate(double sampleRate)
		{
			SRCompressor::setSampleRate(sampleRate);
			mEnvelopeDetectorAverager.setSampleRate(sampleRate);
		}

		//-------------------------------------------------------------
		void SRCompressorRMS::setWindow(double ms)
		{
			mEnvelopeDetectorAverager.setTc(ms);
		}

		//-------------------------------------------------------------
		void SRCompressorRMS::initRuntime(void)
		{
			SRCompressor::initRuntime();
			mAverageOfSquares = DC_OFFSET;
		}

		//-------------------------------------------------------------
		SRLimiter::SRLimiter()
			: mThreshDb(0.0)
			, mThreshLin(1.0)
			, mPeakHoldSamples(0)
			, mPeakHoldTimer(0)
			, mMaxPeak(1.0)
			, mEnvelopeDetectorAttack(1.0)
			, mEnvelopeDetectorRelease(10.0)
			, currentOvershootLin(1.0)
			, mBufferMask(BUFFER_SIZE - 1)
			, mCursor(0)
		{
			setAttack(1.0);
			mOutputBuffer[0].resize(BUFFER_SIZE, 0.0);
			mOutputBuffer[1].resize(BUFFER_SIZE, 0.0);
		}

		//-------------------------------------------------------------
		void SRLimiter::setThresh(double dB)
		{
			mThreshDb = dB;
			mThreshLin = SRPlugins::SRHelpers::DBToAmp(dB);
		}

		//-------------------------------------------------------------
		void SRLimiter::setAttack(double ms)
		{
			unsigned int samp = int(0.001 * ms * mEnvelopeDetectorAttack.getSampleRate());

			assert(samp < BUFFER_SIZE);

			mPeakHoldSamples = samp;
			mEnvelopeDetectorAttack.setTc(ms);
		}

		//-------------------------------------------------------------
		void SRLimiter::setRelease(double ms)
		{
			mEnvelopeDetectorRelease.setTc(ms);
		}

		//-------------------------------------------------------------
		void SRLimiter::setSampleRate(double sampleRate)
		{
			mEnvelopeDetectorAttack.setSampleRate(sampleRate);
			mEnvelopeDetectorRelease.setSampleRate(sampleRate);
		}

		//-------------------------------------------------------------
		void SRLimiter::initRuntime(void)
		{
			mPeakHoldTimer = 0;
			mMaxPeak = mThreshLin;
			currentOvershootLin = mThreshLin;
			mCursor = 0;
			mOutputBuffer[0].assign(BUFFER_SIZE, 0.0);
			mOutputBuffer[1].assign(BUFFER_SIZE, 0.0);
		}

		//-------------------------------------------------------------
		void SRLimiter::FastEnvelope::setCoef(void)
		{
			// rises to 99% of in value over duration of time constant
			mRuntimeCoeff = pow(0.01, (1000.0 / (mTimeConstantMs * mSampleRate)));
		}



		//-------------------------------------------------------------
		SRGate::SRGate()
			: AttRelEnvelope(1.0, 100.0)
			, mThreshDb(0.0)
			, mThreshLin(1.0)
			, currentOvershootLin(DC_OFFSET)
		{
		}

		//-------------------------------------------------------------
		void SRGate::setThresh(double dB)
		{
			mThreshDb = dB;
			mThreshLin = SRPlugins::SRHelpers::DBToAmp(dB);
		}

		//-------------------------------------------------------------
		void SRGate::initRuntime(void)
		{
			currentOvershootLin = DC_OFFSET;
		}

		//-------------------------------------------------------------
		// simple gate with RMS detection
		//-------------------------------------------------------------
		SRGateRms::SRGateRms()
			: mEnvelopeDetectorAverager(5.0)
			, mAverageOfSquares(DC_OFFSET)
		{
		}

		//-------------------------------------------------------------
		void SRGateRms::setSampleRate(double sampleRate)
		{
			SRGate::setSampleRate(sampleRate);
			mEnvelopeDetectorAverager.setSampleRate(sampleRate);
		}

		//-------------------------------------------------------------
		void SRGateRms::setWindow(double ms)
		{
			mEnvelopeDetectorAverager.setTc(ms);
		}

		//-------------------------------------------------------------
		void SRGateRms::initRuntime(void)
		{
			SRGate::initRuntime();
			mAverageOfSquares = DC_OFFSET;
		}


	}
} // end namespace