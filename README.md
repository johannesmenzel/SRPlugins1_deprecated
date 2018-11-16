# Current State

## Release Ready

nothing

## Alpha Ready

### General

* All sections can be bypassed individually
* Manual global AGC (on button click)

### Input and output Stage

* Input gain driven saturation
  * Adding input drive control (0dB .. 60 dB)
  * Putting input fader pre-meter
* Output Safe Pan
* Output Limiter
* Output Clipper

### Filters

* Up to 20th order HP filter
* Biquad filter section
* Switchable LF/HF Bell/Shelf

### Compressor

* Parallel RMS/Peak compression
* Soft knee RMS compressorcompressor
* Compressor Sidechain RMS/Peak compressor

### GUI

* VU Meter
* GR Meter

## To Do

### General

* Add proper oversampling
* Add global AGC
  * Add current algorithm to OnParamChange(all) after improvements of circularBuffer

### Input and output stage

* Improve Gain driven saturation
  * Changing SAT control to "Amount" 

### Filters

* Add up to 20th order LP filter
* Merge filter classes (n-pole)  
  * Changing SROnePole and SRTwoPole back to SRFilter
  * Adding member variable mPoles = (1 .. 2), altered in SRFilter.setType (and .setFilter if not calling)
  * Changing SRFilter.process to conditional processing
* Add constant Q peak filter  
  * Adding asymmetric (non-)constant Q peak filter or adding member initializer bool constantQboost and constantQcut = (false .. true)
* Add other filter classes (bessel...)
* Add filter class type for higher order filters
* Add custom 2-Pole filter (parameters a0, a1 ...)
  * or filter class with morphing 

### Classes

* Add Saturation class
  * Asymetric saturation (even/odd harmonics AND skew)
  * Further evalutation of types (Pirkle tanh/atan; Zoelzer; musicDSP waveshaper; ...)
* Add Circular buffer class

### Compressor

* Improve AGC algorithm
  * Improve time constant transfer function OR
  * Implement circular buffer based RMS in-out detection
* Improve Deesser
  * Add higher order linkwitz-riley filter or add allpass network
* Improve (Switchable) smooth GR-limiter (knee)
* Add feedback/feedforward detector topology

### GUI

* Frequency response graph
* Compressor characteristics graph
* Combined RMS/Peak/VU meter
* Extend range to +12 dBFS
* Channelstrip-like Graphics
* Setting controls position in relation to section position

## Planned Features

### Input and output stage

* Analog component level input and output transformer

### GUI

* Vector grapghic based controls
* Spectral analyser

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
