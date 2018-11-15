/*
 *	Simple Compressor (source)
 *
 *  File		: SimpleComp.cpp
 *	Library		: SimpleSource
 *  Version		: 1.12
 *  Implements	: SimpleComp, SimpleCompRms
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


#include "SimpleComp.h"

namespace SRDynamics
{
	//-------------------------------------------------------------
	// simple compressor
	//-------------------------------------------------------------
	SRCompressor::SRCompressor()	
		: AttRelEnvelope( 10.0, 100.0 )
		, threshdB_( 0.0 )
		, ratio_( 1.0 )
		, currentOvershootDb( DC_OFFSET )
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
		fSidechainFilter1.setFilter(biquad_highpass, mSidechainFc, 0.7071, 0., samplerate);
		fSidechainFilter2.setFilter(biquad_highpass, mSidechainFc, 0.7071, 0., samplerate);
	}

	//-------------------------------------------------------------
	void SRCompressor::setThresh( double dB )
	{
		threshdB_ = dB;
	}

	//-------------------------------------------------------------
	void SRCompressor::setRatio( double ratio )
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
		fSidechainFilter1.setFilter(biquad_highpass, mSidechainFc, 0.7071, 0., getSampleRate());
		fSidechainFilter2.setFilter(biquad_highpass, mSidechainFc, 0.7071, 0., getSampleRate());
	}

	void SRCompressor::setSidechainFilterFreq(double sidechainFc)
	{
		mSidechainFc = sidechainFc;
		fSidechainFilter1.setFc(mSidechainFc);
		fSidechainFilter2.setFc(mSidechainFc);
	}

	//-------------------------------------------------------------
	void SRCompressor::initRuntime( void )
	{
		currentOvershootDb = DC_OFFSET;
	}

	//-------------------------------------------------------------
	// simple compressor with RMS detection
	//-------------------------------------------------------------
	SRCompressorRMS::SRCompressorRMS()
		: ave_( 5.0 )
		, aveOfSqrs_( DC_OFFSET )
	{
	}

	//-------------------------------------------------------------
	void SRCompressorRMS::setSampleRate( double sampleRate )
	{
		SRCompressor::setSampleRate( sampleRate );
		ave_.setSampleRate( sampleRate );
	}

	//-------------------------------------------------------------
	void SRCompressorRMS::setWindow( double ms )
	{
		ave_.setTc( ms );
	}

	//-------------------------------------------------------------
	void SRCompressorRMS::initRuntime( void )
	{
		SRCompressor::initRuntime();
		aveOfSqrs_ = DC_OFFSET;
	}

}	// end namespace chunkware_simple
