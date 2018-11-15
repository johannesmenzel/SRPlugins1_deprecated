#include "SRGenericClass.h"
//#include this and that
//#include <math.h>

// Constructor
SRGenericClass::SRGenericClass() {
    // declare variable values so that non empty
    mType = typeOne;
    mVar1 = 1.;
    mVar2 = 2.;
    mVar3 = 3.;
    mInt1 = 4.;
    mInt2 = 5.;
}

// Member init
SRGenericClass::SRGenericClass(int mType, mVar1, mVar2, mVar3) {
    // set member variables from initializer:
    setClass(pType, pVar1, pVar2, pVar3);
    // set internal values manually:
    mInt1 = 4.;
    mInt2 = 5.;
}

// Destructor
SRGenericClass::~SRGenericClass() {
}

void SRGenericClass::setType(int pType) {
    // set member variable from initializer
    this->mType = pType;
    // in case you need to calculate internal values:
    calcValues();
}

void SRGenericClass::setVar1(double pVar1) {
    // set member variable from initializer
    this->mVar1 = pVar1;
    // in case you need to calculate internal values:
    calcValues();
}


void SRGenericClass::setVar2(double pVar2) {
    // set member variable from initializer
    this->mVar2 = pVar2;
    // in case you need to calculate internal values:
    calcValues();
}


void SRGenericClass::setVar3(double pVar3) {
    // set member variable from initializer
    this->mVar3 = pVar3;
    // in case you need to calculate internal values:
    calcValues();
}

// setClass function takes care that every member variable is initialized on runtime
void SRGenericClass::setClass(int pType, double pVar1, double pVar2, double pVar3) {
    this->mType = pType;
    this->mVar1 = pVar1;
    this->mVar2 = pVar2;
    this->mVar3 = pVar3;
    // in case you need to calculate internal values:
    calcValues();       
}

// Internal calculations. Example on calculation depending on mType:
void SRGenericClass::calcValues(void) {
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