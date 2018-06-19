#ifndef __TRANSCRIBER__
#define __TRANSCRIBER__

#include "IPlug_include_in_plug_hdr.h"
#include "lib_filter/filter.h"

// If this file is missing, run "git_version" script from project folder,
// or create an empty file if you don't use git.
#include "PLUG_Version.h"

#define CONVERT_SAMPLE_RATE(f) (f)
#define CONVERT_LPF_FREQUENCY(f) (f/100.)
#define LOG_TO_LINEAR(v) pow(10, v/20.)
// Gain knob values (dB). The shape's value should be so that the default value is at 12ish o'clock
#define GAIN_KNOB_SHAPE 0.26
#define GAIN_KNOB_MIN -60.
#define GAIN_KNOB_DFT 0.
#define GAIN_KNOB_MAX 12.
#define GAIN_KNOB_STEP .1

// Filter knob values in %. At 100% f_LFP is about 20k, at 50% at 2-5k, at 1% at about 50-100Hz.
// Not accurate, not tied to sample rate, although this application doesn't need that kind of precision. 
#define FILTER_KNOB_SHAPE 1.
#define FILTER_KNOB_MIN 1.
#define FILTER_KNOB_MAX 99.
#define FILTER_KNOB_DFT FILTER_KNOB_MAX
#define FILTER_KNOB_STEP 1.

#define PLUG_VERSION_STRING_LENGTH 96
#ifdef _WIN32
#define PLUG_VERSION_STRING_FONT_SIZE 12
#elif defined(__APPLE__)
#define PLUG_VERSION_STRING_FONT_SIZE 13
#endif

#ifdef _PLUG_VERSION_H
#define PLUG_VERSTION_TEXT \
"Transcriber v%s by Daniel Leonov Plugs | \
danielleonovplugs.com | build %s@%s"
#else
#define PLUG_VERSTION_TEXT \
"Transcriber v%s by Daniel Leonov Plugs | \
danielleonovplugs.com"
#endif

// Number of presets
#define kNumPrograms 1

enum EParams
{
  kCutOffFrequency = 0,
  kGain = 1,
  kSwitch = 2,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kCutOffFrequencyX = 239,
  kCutOffFrequencyY = 150,
  kKnobFrames = 128,
  kSwitchFrames = 2,

  kGainX = 462,
  kGainY = 150,

  kSwitchX = 16,
  kSwitchY = 150,

  kTextVersion_X = 0,
  kTextVersion_Y = 5,
  kTextVersion_W = GUI_WIDTH,
  kTextVersion_H = 30,
  kTextVersion_ColorMono = 91,
};

class Transcriber : public IPlug
{
public:
  Transcriber(IPlugInstanceInfo instanceInfo);
  ~Transcriber();

  void setSampleRate(double sampleRate);
  void setCutOffFrequency(double frequency);

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mCutOffFrequency; // 1.000 = 20ish kHz, 0.001 = all the way off
  double mSampleRate; // in Khz
  double mGain; // in times
  bool mOnOff;
  void CreatePresets();
  Filter* pmFilter;

  IKnobMultiControl *ptCutOffFrequencyControl;
  IKnobMultiControl *ptGainControl;
  ISwitchControl *ptSwitchControl;
};

#endif
