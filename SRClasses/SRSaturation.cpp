#include "SRSaturation.h"
//#include this and that
//#include <math.h>

// Constructor
SRSaturation::SRSaturation() {
    // declare variable values so that non empty
	mType;
	// member variables
	mDriveNormalized = 1.;
	mAmountNormalized = 0.;
	mHarmonicsNormalized = .5;
	mPositive = true;
	mCrosstalkNormalized = 0.;
	mWetNormalized = 1.;
	// internal variables
	in1;
	in2;
	prevSample;
	out;
	in1Dry;
	in2Dry;
}

// Member init
SRSaturation::SRSaturation(int mType, mVar1, mVar2, mVar3) {
    // set member variables from initializer:
    setClass(pType, pVar1, pVar2, pVar3);
    // set internal values manually:
    mInt1 = 4.;
    mInt2 = 5.;
}

// Destructor
SRSaturation::~SRSaturation() {
}

void SRSaturation::setType(int pType) {
    // set member variable from initializer
    this->mType = pType;
    // in case you need to calculate internal values:
    calcValues();
}

void SRSaturation::setVar1(double pVar1) {
    // set member variable from initializer
    this->mVar1 = pVar1;
    // in case you need to calculate internal values:
    calcValues();
}


void SRSaturation::setVar2(double pVar2) {
    // set member variable from initializer
    this->mVar2 = pVar2;
    // in case you need to calculate internal values:
    calcValues();
}


void SRSaturation::setVar3(double pVar3) {
    // set member variable from initializer
    this->mVar3 = pVar3;
    // in case you need to calculate internal values:
    calcValues();
}

// setClass function takes care that every member variable is initialized on runtime
void SRSaturation::setClass(int pType, double pVar1, double pVar2, double pVar3) {
    this->mType = pType;
    this->mVar1 = pVar1;
    this->mVar2 = pVar2;
    this->mVar3 = pVar3;
    // in case you need to calculate internal values:
    calcValues();       
}

// Internal calculations. Example on calculation depending on mType:
void SRSaturation::calcValues(void) {
    switch (this->mType) {
        case typeOne:
            // calculate something
            break;
        case typeTwo:
            // calculate something
            break;
        case typeThree:
            // calculate something
            break;
    }
    return;
}