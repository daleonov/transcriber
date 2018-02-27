#ifndef __TRANSCRIBER__
#define __TRANSCRIBER__

#include "IPlug_include_in_plug_hdr.h"
#include "lib_filter/filter.h"

#define CONVERT_SAMPLE_RATE(f) (f/1000.)

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
