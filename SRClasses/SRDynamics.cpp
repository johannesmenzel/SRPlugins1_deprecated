#include "SRDynamics.h"

//namespace SRClasses {


	SRDynamics::SRDynamics()
	{
	}


	SRDynamics::~SRDynamics()
	{
	}


	/*##########################################################
						ENVELOPE DETECTOR
	##########################################################*/

	SREnvelopeDetector::SREnvelopeDetector(double ms, double sampleRate)
	{
		assert(sampleRate > 0.0);
		assert(ms > 0.0);
		sampleRate_ = sampleRate;
		ms_ = ms;
		setCoef();
	}

	//-------------------------------------------------------------
	void SREnvelopeDetector::setTc(double ms)
	{
		//assert( ms > 0.0 );
		ms_ = ms;
		setCoef();
	}

	//-------------------------------------------------------------
	void SREnvelopeDetector::setSampleRate(double sampleRate)
	{
		assert(sampleRate > 0.0);
		sampleRate_ = sampleRate;
		setCoef();
	}

	//-------------------------------------------------------------
	void SREnvelopeDetector::setCoef(void)
	{
		coef_ = exp(-1000.0 / (ms_ * sampleRate_));
	}



	/*##########################################################
						ATTACK/RELEASE ENVELOPE
	##########################################################*/

	SRAttRelEnvelope::SRAttRelEnvelope(double att_ms, double rel_ms, double sampleRate)
		: att_(att_ms, sampleRate)
		, rel_(rel_ms, sampleRate)
	{
	}

	//-------------------------------------------------------------
	void SRAttRelEnvelope::setAttack(double ms)
	{
		att_.setTc(ms);
	}

	//-------------------------------------------------------------
	void SRAttRelEnvelope::setRelease(double ms)
	{
		rel_.setTc(ms);
	}

	//-------------------------------------------------------------
	void SRAttRelEnvelope::setSampleRate(double sampleRate)
	{
		att_.setSampleRate(sampleRate);
		rel_.setSampleRate(sampleRate);
	}


	/*##########################################################
							COMPRESSOR
	##########################################################*/

	SRCompressor::SRCompressor()
		: SRAttRelEnvelope(10.0, 100.0)
		, threshdB_(0.0)
		, ratio_(1.0)
		, envdB_(DC_OFFSET)
	{
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
	}

	//-------------------------------------------------------------
	void SRCompressor::initRuntime(void)
	{
		envdB_ = DC_OFFSET;
	}

	/*##########################################################
							RMS COMPRESSOR
	##########################################################*/

	SRCompressorRms::SRCompressorRms()
		: ave_(5.0)
		, aveOfSqrs_(DC_OFFSET)
	{
	}

	//-------------------------------------------------------------
	void SRCompressorRms::setSampleRate(double sampleRate)
	{
		SRCompressorRms::setSampleRate(sampleRate);
		ave_.setSampleRate(sampleRate);
	}
	
	//-------------------------------------------------------------
	void SRCompressorRms::setWindow(double ms)
	{
		ave_.setTc(ms);
	}

	//-------------------------------------------------------------
	void SRCompressorRms::initRuntime(void)
	{
		SRCompressor::initRuntime();
		aveOfSqrs_ = DC_OFFSET;
	}

	/*##########################################################
								GATE
	##########################################################*/

	//-------------------------------------------------------------
	SRGate::SRGate()
		: SRAttRelEnvelope(1.0, 100.0)
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

	/*##########################################################
								RMS GATE
	##########################################################*/

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


	/*##########################################################
								LIMITER
	##########################################################*/

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

//} // end namespace