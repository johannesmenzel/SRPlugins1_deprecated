# Current State

## Release Ready

_nothing_

## Alpha Ready

### General

* All sections can be __bypassed individually__
* Manual global __AGC__ (on button click)

### Input and output Stage

* Input __gain driven saturation__
  * Adding input drive control (0dB .. 60 dB)
  * Putting input fader pre-meter
* Output __Safe Pan__
* Output __Limiter__
* Output __Clipper__

### Filters

* Up to __20th order highpass__ filter
* __Biquad__ filter section
* Switchable LF/HF __Bell/Shelf__

### Compressor

* __Parallel__ RMS/Peak compression
* __Soft knee__ RMS/Peak compressor
* Smooth __gain reduction limiter__ RMS/Peak compressor
* __External sidechain__ RMS/Peak compressor
* __Sidechain filter__ for Peak compressor

### GUI

* __VU__ Meter
* __Gain Reduction__ Meter

## To Do

### General

* Add proper __oversampling__
* Improve global __AGC__
  * Add current algorithm to OnParamChange(all) after improvements of circularBuffer

### Input and output stage

* Improve __Gain driven saturation__
  * Changing SAT control to "Amount" 

### Filters

* Add up to __20th order lowpass filter__
* Merge __filter classes__ (n-pole)  
  * Changing SROnePole and SRTwoPole back to SRFilter
  * Adding member variable mPoles = (1 .. 2), altered in SRFilter.setType (and .setFilter if not calling)
  * Changing SRFilter.process to conditional processing
* Add __constant Q peak__ filter  
  * Adding asymmetric (non-)constant Q peak filter or adding member initializer bool constantQboost and constantQcut = (false .. true)
* Add other __filter classes__ (bessel...)
* Add filter class type for __higher order filters__
* Add __custom 2-Pole__ filter (parameters a0, a1 ...)
  * or filter class with morphing 

### Classes

* Add __Saturation__ class
  * Asymetric saturation (even/odd harmonics AND skew)
  * Further evalutation of types (Pirkle tanh/atan; Zoelzer; musicDSP waveshaper; ...)
* Add __Circular buffer__ class

### Compressor

* Improve __AGC__ algorithm
  * Improve time constant transfer function OR
  * Implement circular buffer based RMS in-out detection
* Improve __Deesser__
  * Add higher order linkwitz-riley filter or add allpass network
* Improve (Switchable) __smooth GR-limiter__ (knee)
* Add __feedback/feedforward__ detector topology
* Add __sidechain filter__ for RMS compressor

### GUI

* Add controls
  * Add smaller control for minor parameters
  * Add __frequency response__ graph
  * Improve meter section
    * Combined RMS/Peak/__VU__ meter
    * Extend __meter range__ to +12 dBFS
    * Improve VU meter __envelope coefficients__
* Add Channelstrip-like Graphics
* Setting controls position in relation to section position

## Planned Features

### Input and output stage

* Analog component level input and output transformer

### GUI

* Vector grapghic based controls
* Spectral analyser
* Compressor characteristics graph

---

# Compile

Clone this repository to your wdl-ol folders first level (not into any subdirectory).

---

# External libraries used or modified

* WDL-OL framework
* EarLevels Biquad c++ class
* chunkware simple source dynamics classes
* _IPlug2 oversampling class_

_italic_: not implemented yet

(may not be complete yet)
