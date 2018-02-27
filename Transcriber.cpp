#include "Transcriber.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "lib_filter/filter.h"

const int kNumPrograms = 5;

enum EParams
{
  kCutOffFrequency = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kCutOffFrequencyX = 79,
  kCutOffFrequencyY = 62,
  kKnobFrames = 128
};

Transcriber::Transcriber(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mCutOffFrequency(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kCutOffFrequency)->InitDouble("CutOffFrequency", .99, 0.01, .99, 0.001);
  GetParam(kCutOffFrequency)->SetShape(2);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  //pGraphics->AttachPanelBackground(&COLOR_RED);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kCutOffFrequencyX, kCutOffFrequencyY, kCutOffFrequency, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  //MakeDefaultPreset((char *) "-", kNumPrograms);

  setSampleRate(CONVERT_SAMPLE_RATE(GetSampleRate()));
  pmFilter = new Filter();
  pmFilter->setCutoff(.5);
  CreatePresets();
}

Transcriber::~Transcriber() {
  //delete pmFilter;

}


void Transcriber::setSampleRate(double sampleRate) {
  mSampleRate = sampleRate;
}
void Transcriber::setCutOffFrequency(double frequency) {
  mCutOffFrequency = frequency;
}

void Transcriber::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double *leftOutput = outputs[0];
  double *rightOutput = outputs[1];

  double *leftInput = inputs[0];
  double *rightInput = inputs[1];

  double fCurrentSample;

  for (int i = 0; i < nFrames; ++i) {

    fCurrentSample = leftInput[i] - rightInput[i];
    leftOutput[i] = rightOutput[i] = pmFilter->process(fCurrentSample);

  }
}

void Transcriber::CreatePresets() {
  MakePreset("default", 15.0);
}

void Transcriber::Reset()
{
  TRACE;
  IMutexLock lock(this);
  setSampleRate(CONVERT_SAMPLE_RATE(GetSampleRate()));
  //delete pmFilter;
  //pmFilter = new Filter(LPF, 51, mSampleRate, 2.0);
}

void Transcriber::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kCutOffFrequency:
      pmFilter->setCutoff(GetParam(kCutOffFrequency)->Value());
      break;

    default:
      break;
  }
}
