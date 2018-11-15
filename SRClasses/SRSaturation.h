//  SRSaturation.h

// Implementation:
//   Note that you might need a class for each channel
//   (so 2 for stereo processing.)
//
// Header:
//   private:
//     Impementation as object:
//       SRSaturation name;
//     Implementation as pointer:
//       SRSaturation *name = new SRSaturation();
//
// Class: Constructor, Reset()
//     Implementation as object:
//       name.setClass(pType, pVar1, pVar2, pVar3); 
//     Implementation as pointer:
//       name->setClass(pType, pVar1, pVar2, pVar3);
//
// Class: ProcessDoubleReplacing()
//   Per sample and channel:
//     Implementation as object:
//       *out1 = name.process(*in1);
//     Implementation as pointer:
//       *out1 = name->process(*in1);

#ifndef SRSaturation_h
#define SRSaturation_h

// #include this and that

// If type definitions of type float needed:
enum {
    typeMusicDSP = 0,
    typeZoelzer,
    typePirkle
    // ...
};

class SRSaturation {
public:
    // constructor
    SRSaturation();
    // class initializer
    SRSaturation(
        int pType,
		double pDriveNormalized,
		double pAmountNormalized,
		double pHarmonicsNormalized,
		bool pPositiveSide,
		double pCrosstalkNormalized,
		double pWet
	);
    // destructor
    ~SRSaturation(); // destructor

    // public functions that need to be accessed from outside
    void setType(int pType);
	void setDrive(double pDriveNormalized);
	void setAmount(double pAmountNormalized);
	void setHarmonics(double pHarmonicsNormalized);
	void setPositive(bool pPositive);
	void setCrosstalk(double pCrosstalkNormalized);
	void setWet(double pWetNormalized);

	void setClass(
		int pType,
		double pDriveNormalized,
		double pAmountNormalized,
		double pHarmonicsNormalized,
		double pPositive,
		double pCrosstalkNormalized,
		double pWetNormalized
	);
    // inline process function, if needed
    double process(double in);

protected:
    // Protected functions that do internal calculations and that are called from other funcions
    void calcValues(void);

    // Internal member and internal variables
    int mType;
	// member variables
	double mDriveNormalized;
	double mAmountNormalized;
	double mHarmonicsNormalized;
	bool mPositive;
	double mCrosstalkNormalized;
	double mWetNormalized;
	// internal variables
	double in1;
	double in2;
	double prevSample;
	double out;
	double in1Dry;
	double in2Dry;

}; // end of class

inline double SRSaturation::process(double in1, double in2) {
	out = in1; // and additional processing
	prevSample = in1
    return out;
}

#endif // SRSaturation_h