#ifndef __TRANSCRIBER__
#define __TRANSCRIBER__

#include "IPlug_include_in_plug_hdr.h"
#include "lib_filter/filter.h"

#define CONVERT_SAMPLE_RATE(f) (f)
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

  kTextVersion_X = 8,
  kTextVersion_Y = 418,
  kTextVersion_W = 100,
  kTextVersion_H = 20,
  kTextVersion_ColorMono = 70,
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
};

#endif
