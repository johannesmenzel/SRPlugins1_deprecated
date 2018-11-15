//
//  Biquad.cpp
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include "SRFilters.h"
//#include "../SRChannel/SRChannel.h"



	SRFiltersTwoPole::SRFiltersTwoPole() {
		type = biquad_lowpass;
		a0 = 1.0;
		a1 = a2 = b1 = b2 = 0.0;
		Fc = 0.50;
		Q = 0.70710678;
		peakGain = 0.0;
		z1 = z2 = 0.0;
		samplerate = 44100;
	}

	SRFiltersTwoPole::SRFiltersTwoPole(int type, double Fc, double Q, double peakGainDB, double samplerate) {
		setFilter(type, Fc, Q, peakGainDB, samplerate);
		z1 = z2 = 0.0;
	}

	SRFiltersTwoPole::~SRFiltersTwoPole() {
	}

	void SRFiltersTwoPole::setType(int type) {
		this->type = type;
		calcBiquad();
	}

	void SRFiltersTwoPole::setQ(double Q) {
		this->Q = Q;
		calcBiquad();
	}

	void SRFiltersTwoPole::setFc(double Fc) {
		this->Fc = Fc;
		calcBiquad();
	}

	void SRFiltersTwoPole::setPeakGain(double peakGainDB) {
		this->peakGain = peakGainDB;
		calcBiquad();
	}

	double SRFiltersTwoPole::getFreqResp(double plotFc) {
		double w = M_PI * (plotFc / samplerate);
		return 20 * log((sqrt(pow(a0*pow(cos(w), 2) - a0 * pow(sin(w), 2) + a1 * cos(w) + a2, 2) + pow(2 * a0*cos(w)*sin(w) + a1 * (sin(w)), 2)) /
			sqrt(pow(pow(cos(w), 2) - pow(sin(w), 2) + b1 * cos(w) + b2, 2) + pow(2 * cos(w)*sin(w) + b1 * (sin(w)), 2))));
	}
    
	void SRFiltersTwoPole::setFilter(int type, double Fc, double Q, double peakGainDB, double samplerate) {
		this->type = type;
		this->Q = Q;
		this->Fc = Fc;
		this->samplerate = samplerate;
		setPeakGain(peakGainDB);
	}

	void SRFiltersTwoPole::calcBiquad(void) {
		double norm, O, K, V;
		double L, E, B, G;
		switch (this->type) {
			case biquad_lowpass:
				K = tan(M_PI * Fc);
				norm = 1 / (1 + K / Q + K * K);
				a0 = K * K * norm;
				a1 = 2 * a0;
				a2 = a0;
				b1 = 2 * (K * K - 1) * norm;
				b2 = (1 - K / Q + K * K) * norm;
				break;
            
			case biquad_highpass:
				K = tan(M_PI * Fc);
				norm = 1 / (1 + K / Q + K * K);
				a0 = 1 * norm;
				a1 = -2 * a0;
				a2 = a0;
				b1 = 2 * (K * K - 1) * norm;
				b2 = (1 - K / Q + K * K) * norm;
				break;
            
			case biquad_bandpass:
				K = tan(M_PI * Fc);
				norm = 1 / (1 + K / Q + K * K);
				a0 = K / Q * norm;
				a1 = 0;
				a2 = -a0;
				b1 = 2 * (K * K - 1) * norm;
				b2 = (1 - K / Q + K * K) * norm;
				break;
            
			case biquad_notch:
				K = tan(M_PI * Fc);
				norm = 1 / (1 + K / Q + K * K);
				a0 = (1 + K * K) * norm;
				a1 = 2 * (K * K - 1) * norm;
				a2 = a0;
				b1 = a1;
				b2 = (1 - K / Q + K * K) * norm;
				break;
            
			case biquad_peak:
				V = pow(10, fabs(peakGain) / 20.0);
				K = tan(M_PI * Fc);
				if (peakGain >= 0) {    // boost
					norm = 1 / (1 + 1/Q * K + K * K);
					a0 = (1 + V/Q * K + K * K) * norm;
					a1 = 2 * (K * K - 1) * norm;
					a2 = (1 - V/Q * K + K * K) * norm;
					b1 = a1;
					b2 = (1 - 1/Q * K + K * K) * norm;
				}
				else {    // cut
					norm = 1 / (1 + V/Q * K + K * K);
					a0 = (1 + 1/Q * K + K * K) * norm;
					a1 = 2 * (K * K - 1) * norm;
					a2 = (1 - 1/Q * K + K * K) * norm;
					b1 = a1;
					b2 = (1 - V/Q * K + K * K) * norm;
				}
				break;
	
			case biquad_peak_ncq:
				V = pow(10, fabs(peakGain) / 20.0);
				L = 2 * M_PI * Fc;
				E = 4 / (1 + V);
				B = .5 * ((1 - E * tan(L / 2 * Q)) / (1 + E * tan(L / 2 * Q)));
				G = (.5 + B) * cos(L);
				if (peakGain >= 0) {    // boost
					norm = 1 / (1 + 1 / Q * L + L * L);
					a0 = .5 - B * norm;
					a1 = 0. * norm;
					a2 = -(.5 - B) * norm;
					b1 = -2 * G * norm;
					b2 = 2 * B * norm;
				}
				else {    // cut
					norm = 1 / (1 + V / Q * L + L * L);
					a0 = .5 - B * norm;
					a1 = 0. * norm;
					a2 = -(.5 - B) * norm;
					b1 = -2 * G * norm;
					b2 = 2 * B * norm;
				}
				break;
	

			case biquad_lowshelf:
				V = pow(10, fabs(peakGain) / 20.0);
				K = tan(M_PI * Fc);
				if (peakGain >= 0) {    // boost
					norm = 1 / (1 + sqrt(2) * K + K * K);
					a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
					a1 = 2 * (V * K * K - 1) * norm;
					a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
					b1 = 2 * (K * K - 1) * norm;
					b2 = (1 - sqrt(2) * K + K * K) * norm;
				}
				else {    // cut
					norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
					a0 = (1 + sqrt(2) * K + K * K) * norm;
					a1 = 2 * (K * K - 1) * norm;
					a2 = (1 - sqrt(2) * K + K * K) * norm;
					b1 = 2 * (V * K * K - 1) * norm;
					b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
				}
				break;

			case biquad_highshelf:
				V = pow(10, fabs(peakGain) / 20.0);
				K = tan(M_PI * Fc);
				if (peakGain >= 0) {    // boost
					norm = 1 / (1 + sqrt(2) * K + K * K);
					a0 = (V + sqrt(2*V) * K + K * K) * norm;
					a1 = 2 * (K * K - V) * norm;
					a2 = (V - sqrt(2*V) * K + K * K) * norm;
					b1 = 2 * (K * K - 1) * norm;
					b2 = (1 - sqrt(2) * K + K * K) * norm;
				}
				else {    // cut
					norm = 1 / (V + sqrt(2*V) * K + K * K);
					a0 = (1 + sqrt(2) * K + K * K) * norm;
					a1 = 2 * (K * K - 1) * norm;
					a2 = (1 - sqrt(2) * K + K * K) * norm;
					b1 = 2 * (K * K - V) * norm;
					b2 = (V - sqrt(2*V) * K + K * K) * norm;
				}
				break;

			case iir_linkwitz_highpass:
				O = M_PI * Fc * samplerate; // OMEGAc from Pirkle, p. 186
				K = O / tan(M_PI * Fc);
				norm = 1 / (K * K + pow(O, 2) + 2 * K * O);
				a0 = (K * K) * norm;
				a1 = (-2 * (K * K)) * norm;
				a2 = a0;
				b1 = (-2 * K * K + 2 * O * O) * norm;
				b2 = (-2 * K * O + K * K + O * O) * norm;
				break;

			case iir_linkwitz_lowpass:
				O = M_PI * Fc * samplerate; // OMEGAc from Pirkle, p. 186
				K = O / tan(M_PI * Fc);
				norm = 1 / (K * K + pow(O, 2) + 2 * K * O);
				a0 = (O * O) * norm;
				a1 = 2 * ((O * O) * norm);
				a2 = a0;
				b1 = (-2 * K * K + 2 * O * O) * norm;
				b2 = (-2 * K * O + K * K + O * O) * norm;
				break;

		}
    



		return;
	}

	SRFiltersOnePole::SRFiltersOnePole() {
		a0 = 1.0;
		b1 = 0.0;
		z1 = 0.0;
	}

	SRFiltersOnePole::SRFiltersOnePole(int type, double Fc, double samplerate) {
		setFilter(type, Fc, samplerate);
		z1 = 0.0;
		setType(type);
		setFc(Fc);
	}

	void SRFiltersOnePole::setFilter(int type, double Fc, double samplerate) {
		this->type = type;
		this->Fc = Fc;
		this->samplerate = samplerate;
	}

	void SRFiltersOnePole::setType(int type) {
		this->type = type;
		calcOnePole();
	}

	void SRFiltersOnePole::setFc(double Fc) {
		this->Fc = Fc;
		calcOnePole();
	}

	void SRFiltersOnePole::calcOnePole(void) {
		b1 = exp(-2.0 * M_PI * Fc);
		a0 = 1.0 - b1;
		//	b1 = -exp(-2.0 * M_PI * (0.5 - Fc));
		//	a0 = 1.0 + b1;
	}