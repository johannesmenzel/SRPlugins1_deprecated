//  SRGenericClass.h

// Implementation:
//   Note that you might need a class for each channel
//   (so 2 for stereo processing.)
//
// Header:
//   private:
//     Impementation as object:
//       SRGenericClass name;
//     Implementation as pointer:
//       SRGenericClass *name = new SRGenericClass();
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

#ifndef SRGenericClass_h
#define SRGenericClass_h

// #include this and that

// If type definitions of type float needed:
enum {
    typeOne = 0,
    typeTwo,
    typeThree
    // ...
};

class SRGenericClass {
public:
    // constructor
    SRGenericClass();
    // class initializer
    SRGenericClass(
        int pType,
        double pVar1,
        double pVar2,
        double pVar3
        );
    // destructor
    ~SRGenericClass(); // destructor

    // public functions that need to be accessed from outside
    void setType(int pType);
    void setVar1(double pVar1);
    void setVar2(double pVar2);
    void setVar3(double pVar3);
    void setClass(
        int pType,
        double pVar1,
        double pVar2,
        double pVar3
        );
    // inline process function, if needed
    double process(double in);

protected:
    // Protected functions that do internal calculations and that are called from other funcions
    void calcValues(void);

    // Internal member and internal variables
    int mType;
    double mVar1, mVar2, mVar3; // member variables
    double mInt1, mInt2; // internal variables

}; // end of class

inline double SRGenericClass::process(double in) {
    double out = in // and additional processing
    return out;
}

#endif // SRGenericClass_h