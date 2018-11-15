#include "SRDynamics.h"

namespace SRDynamics {

	//-------------------------------------------------------------
	// envelope detector
	//-------------------------------------------------------------
	EnvelopeDetector::EnvelopeDetector(double ms, double sampleRate)
	{
		assert(sampleRate > 0.0);
		assert(ms > 0.0);
		sampleRate_ = sampleRate;
		ms_ = ms;
		setCoef();
	}

	

	//-------------------------------------------------------------
	void EnvelopeDetector::setTc(double ms)
	{
		//assert( ms > 0.0 );
		ms_ = ms;
		setCoef();
	}

	//-------------------------------------------------------------
	void EnvelopeDetector::setSampleRate(double sampleRate)
	{
		assert(sampleRate > 0.0);
		sampleRate_ = sampleRate;
		setCoef();
	}

	//-------------------------------------------------------------
	void EnvelopeDetector::setCoef(void)
	{
		coef_ = exp(-1000.0 / (ms_ * sampleRate_));
	}

	//-------------------------------------------------------------
	// attack/release envelope
	//-------------------------------------------------------------
	AttRelEnvelope::AttRelEnvelope(double att_ms, double rel_ms, double sampleRate)
		: att_(att_ms, sampleRate)
		, rel_(rel_ms, sampleRate)
	{
	}

	//-------------------------------------------------------------
	void AttRelEnvelope::setAttack(double ms)
	{
		att_.setTc(ms);
	}

	//-------------------------------------------------------------
	void AttRelEnvelope::setRelease(double ms)
	{
		rel_.setTc(ms);
	}

	//-------------------------------------------------------------
	void AttRelEnvelope::setSampleRate(double sampleRate)
	{
		att_.setSampleRate(sampleRate);
		rel_.setSampleRate(sampleRate);
	}



		//-------------------------------------------------------------
		// simple compressor
		//-------------------------------------------------------------
		SRCompressor::SRCompressor()
			: AttRelEnvelope(10.0, 100.0)
			, threshdB_(0.0)
			, ratio_(1.0)
			, currentOvershootDb(DC_OFFSET)
			, mSidechainFc(0.0)
			, grLin(1.0)
			, grDb(0.0)
			, kneeDb_(0.0)
			, maxGr(0.0)
		{
		}

		void SRCompressor::setCompressor(double dB, double ratio, double attackMs, double releaseMs, double sidechainFc, double kneeDb, double samplerate) {
			setThresh(dB);
			setRatio(ratio);
			setAttack(attackMs);
			setRelease(releaseMs);
			setSidechainFilterFreq(sidechainFc);
			setSampleRate(samplerate);
			setKnee(kneeDb);
			fSidechainFilter1.setFilter(SRFilters::biquad_highpass, mSidechainFc, 0.7071, 0., samplerate);
			fSidechainFilter2.setFilter(SRFilters::biquad_highpass, mSidechainFc, 0.7071, 0., samplerate);
		}

		//-------------------------------------------------------------
		void SRCompressor::setThresh(double dB)
		{
			threshdB_ = dB;
		}

		//-------------------------------------------------------------
		void SRCompressor::setRatio(double ratio)
		{
			//assert( ratio > 0.0 );
			ratio_ = ratio;
			maxGr = 73.4979484210802 - 88.939188010773 * (1 - exp(-1.75091102973106 * (1 / ratio)));
		}

		void SRCompressor::setKnee(double kneeDb)
		{
			//assert( ratio > 0.0 );
			kneeDb_ = kneeDb;
		}

		void SRCompressor::setSidechainFilter(double sidechainFc) {
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
			: ave_(5.0)
			, aveOfSqrs_(DC_OFFSET)
		{
		}

		//-------------------------------------------------------------
		void SRCompressorRMS::setSampleRate(double sampleRate)
		{
			SRCompressor::setSampleRate(sampleRate);
			ave_.setSampleRate(sampleRate);
		}

		//-------------------------------------------------------------
		void SRCompressorRMS::setWindow(double ms)
		{
			ave_.setTc(ms);
		}

		//-------------------------------------------------------------
		void SRCompressorRMS::initRuntime(void)
		{
			SRCompressor::initRuntime();
			aveOfSqrs_ = DC_OFFSET;
		}

		//-------------------------------------------------------------
		SRLimiter::SRLimiter()
			: threshdB_(0.0)
			, thresh_(1.0)
			, peakHold_(0)
			, peakTimer_(0)
			, maxPeak_(1.0)
			, att_(1.0)
			, rel_(10.0)
			, env_(1.0)
			, mask_(BUFFER_SIZE - 1)
			, cur_(0)
		{
			setAttack(1.0);
			outBuffer_[0].resize(BUFFER_SIZE, 0.0);
			outBuffer_[1].resize(BUFFER_SIZE, 0.0);
		}

		//-------------------------------------------------------------
		void SRLimiter::setThresh(double dB)
		{
			threshdB_ = dB;
			thresh_ = dB2lin(dB);
		}

		//-------------------------------------------------------------
		void SRLimiter::setAttack(double ms)
		{
			unsigned int samp = int(0.001 * ms * att_.getSampleRate());

			assert(samp < BUFFER_SIZE);

			peakHold_ = samp;
			att_.setTc(ms);
		}

		//-------------------------------------------------------------
		void SRLimiter::setRelease(double ms)
		{
			rel_.setTc(ms);
		}

		//-------------------------------------------------------------
		void SRLimiter::setSampleRate(double sampleRate)
		{
			att_.setSampleRate(sampleRate);
			rel_.setSampleRate(sampleRate);
		}

		//-------------------------------------------------------------
		void SRLimiter::initRuntime(void)
		{
			peakTimer_ = 0;
			maxPeak_ = thresh_;
			env_ = thresh_;
			cur_ = 0;
			outBuffer_[0].assign(BUFFER_SIZE, 0.0);
			outBuffer_[1].assign(BUFFER_SIZE, 0.0);
		}

		//-------------------------------------------------------------
		void SRLimiter::FastEnvelope::setCoef(void)
		{
			// rises to 99% of in value over duration of time constant
			coef_ = pow(0.01, (1000.0 / (ms_ * sampleRate_)));
		}



		//-------------------------------------------------------------
		SRGate::SRGate()
			: AttRelEnvelope(1.0, 100.0)
			, threshdB_(0.0)
			, thresh_(1.0)
			, env_(DC_OFFSET)
		{
		}

		//-------------------------------------------------------------
		void SRGate::setThresh(double dB)
		{
			threshdB_ = dB;
			thresh_ = dB2lin(dB);
		}

		//-------------------------------------------------------------
		void SRGate::initRuntime(void)
		{
			env_ = DC_OFFSET;
		}

		//-------------------------------------------------------------
		// simple gate with RMS detection
		//-------------------------------------------------------------
		SRGateRms::SRGateRms()
			: ave_(5.0)
			, aveOfSqrs_(DC_OFFSET)
		{
		}

		//-------------------------------------------------------------
		void SRGateRms::setSampleRate(double sampleRate)
		{
			SRGate::setSampleRate(sampleRate);
			ave_.setSampleRate(sampleRate);
		}

		//-------------------------------------------------------------
		void SRGateRms::setWindow(double ms)
		{
			ave_.setTc(ms);
		}

		//-------------------------------------------------------------
		void SRGateRms::initRuntime(void)
		{
			SRGate::initRuntime();
			aveOfSqrs_ = DC_OFFSET;
		}


} // end namespace