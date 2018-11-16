# Current State

## Release Ready

* none

---

## Alpha Ready

### General

* All sections can be bypassed individually
* Manual global AGC (on button click)

### Input Stage

* Gain driven saturation
  		\- Adding input drive control (0dB .. 60 dB)
  		\- Putting input fader pre-meter

### Filters

* Up to 20th order HP filter
* Biquad filter section
* Switchable LF/HF Bell/Shelf

### Compressor

* Parallel RMS/Peak Compression
* Soft knee compressor

### Output Stage

* Safe Pan
* Output Limiter
* Output Clipper

### GUI

* VU Meter
* GR Meter

---

## To Do

### General

* Implement proper oversampling
* Implement global AGC

### Input Stage

* Gain driven saturation
  		\- Changing SAT control to "Amount" 
* Component level input transformer

### Filters

* Up to 20th order LP filter
* Merge filter classes (n-pole)
  		\- Changing SROnePole and SRTwoPole back to SRFilter
  		\- Adding member variable mPoles = (1 .. 2), altered in SRFilter.setType (and .setFilter if not calling)
  		\- Changing SRFilter.process to conditional processing
* Adding constant Q peak filter
  		\- Adding asymmetric (non-)constant Q peak filter or adding member initializer bool constantQboost and constantQcut = (false .. true)
* Other filter classes (bessel...)
* Adding custom 2-Pole filter (parameters a0, a1 ...)
  		\- or filter class with morphing 

### Classes

* Saturation class
* Circular buffer class

### Compressor

* AGC algorithm
* Deesser
* (Switchable) smooth GR-limiter (knee)

### Output Stage

* Component level output transformer

### GUI

* Frequency response graph
* Spectral analyser
* Compressor characteristics graph
* Combined RMS/Peak/VU meter
* Channelstrip-like Graphics
* Setting controls position in relation to section position


# Compile

Clone this repository to your wdl-ol folders first level (not into any subdirectory).

# External libraries used or modified

* WDL-OL framework
* EarLevels Biquad c++ class
* chunkware simple source dynamics classes

(may not be complete yet)
