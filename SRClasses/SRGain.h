//  SRGain.h

// Implementation:
//   Note that you might need a class for each channel
//   (so 2 for stereo processing.)
//
// Header:
//   private:
//     Impementation as object:
//       SRPan name;
//     Implementation as pointer:
//       SRPan *name = new SRPan();
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

#ifndef SRGain_h
#define SRGain_h

// #include this and that
#include "SRHelpers.h" // optional

namespace SRPlugins
{
namespace SRGain
{

// If type definitions of type int needed:
enum
{
	typeLinear = 0,
	typeSquareroot,
	typeSinusodial,
	typeTanh,
	numTypes
	// ...
};

class SRPan
{
  public:
	// constructor
	SRPan();
	// class initializer
	SRPan(
		int pType,
		double pPanNormalized,
		bool pLinearMiddlePosition);
	// destructor
	~SRPan(); // destructor

	// public functions that need to be accessed from outside
	void setType(int pType);
	void setLinearMiddlePosition(bool pLinearMiddlePosition);
	void setPanPosition(double pPanNormalized); // create these for every member
	void getPanPosition() { return mPanNormalized; }

	void setPan(
		int pType,
		double pPanNormalized,
		bool pLinearMiddlePosition);
	// inline process functions, if needed
	// void process(double &in);				// for mono
	void process(double &in1, double &in2); // for stereo

  protected:
	// Protected functions that do internal calculations and that are called from other funcions
	void calcPan(void);

	// Internal member and internal variables
	int mType;
	// member variables
	double mPanNormalized; // Enter normalized pan between 0.0 (left) and 1.0 (right)
	bool mLinearMiddlePosition;
	// internal variables
	double gain1, gain2;
	// double prev[2];
	// double dry[2];
}; // end of class

// inline void SRPan::process(double &in)
// {
// 	process()
// }

inline void SRPan::process(double &in1, double &in2)
{
	if (mPanNormalized != 0.5)
	{
		in1 *= gain1;
		in2 *= gain2;
	}
}

} // namespace SRGain
} // namespace SRPlugins

#endif // SRGain_h