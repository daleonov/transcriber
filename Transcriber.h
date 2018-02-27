#ifndef __TRANSCRIBER__
#define __TRANSCRIBER__

#include "IPlug_include_in_plug_hdr.h"
#include "lib_filter/filter.h"

#define CONVERT_SAMPLE_RATE(f) (f)
#define kNumPrograms 2

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
  double mCutOffFrequency; // in Khz
  double mSampleRate; // in Khz
  void CreatePresets();
  Filter* pmFilter;
};

#endif
