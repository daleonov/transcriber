#include "Transcriber.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "lib_filter/filter.h"
#include "DLPG_FeedbackSender.h"

Transcriber::Transcriber(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mCutOffFrequency(1.)
{
  TRACE;
  mOnOff = true;
  mGain = LOG_TO_LINEAR(GAIN_KNOB_DFT);
  char sDisplayedVersion[PLUG_VERSION_STRING_LENGTH];

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kCutOffFrequency)->InitDouble("CutOffFrequency", FILTER_KNOB_DFT, FILTER_KNOB_MIN, FILTER_KNOB_MAX, FILTER_KNOB_STEP, "%");
  GetParam(kCutOffFrequency)->SetShape(FILTER_KNOB_SHAPE);
  GetParam(kGain)->InitDouble("Gain", GAIN_KNOB_DFT, GAIN_KNOB_MIN, GAIN_KNOB_MAX, GAIN_KNOB_STEP, "dB");
  GetParam(kGain)->SetShape(GAIN_KNOB_SHAPE);
  GetParam(kSwitch)->InitBool("OnOff", 1, "OnOff");

  // Background
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  // Knobs and switch
  IBitmap tGuiBmp = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  ptGainControl = new IKnobMultiControl(this, kGainX, kGainY, kGain, &tGuiBmp);
  tGuiBmp = pGraphics->LoadIBitmap(KNOB_FILTER_ID, KNOB_FILTER_FN, kKnobFrames);
  ptCutOffFrequencyControl = new IKnobMultiControl(this, kCutOffFrequencyX, kCutOffFrequencyY, kCutOffFrequency, &tGuiBmp);
  tGuiBmp = pGraphics->LoadIBitmap(SWITCH_ID, SWITCH_FN, kSwitchFrames);
  ptSwitchControl = new ISwitchControl(this, kSwitchX, kSwitchY, kSwitchFrames, &tGuiBmp);
  pGraphics->AttachControl(ptCutOffFrequencyControl);
  pGraphics->AttachControl(ptGainControl);
  pGraphics->AttachControl(ptSwitchControl);

  // Text string with current version
  #ifdef _PLUG_VERSION_H
  sprintf(
    sDisplayedVersion,
    PLUG_VERSTION_TEXT,
    VST3_VER_STR,
    &sPlugVersionGitHead,
    &sPlugVersionDate
    );
  #else
  sprintf(
    sDisplayedVersion,
    PLUG_VERSTION_TEXT,
    VST3_VER_STR
    );
  #endif

  IText tTextVersion = IText(PLUG_VERSION_STRING_LENGTH);
  IRECT tTextVersionIrect = IRECT(
    kTextVersion_X,
    kTextVersion_Y,
    (kTextVersion_X + kTextVersion_W),
    (kTextVersion_Y + kTextVersion_H)
    );
  const IColor tTextVersionColor(255, kTextVersion_ColorMono, kTextVersion_ColorMono, kTextVersion_ColorMono);
  tTextVersion.mSize = PLUG_VERSION_STRING_FONT_SIZE;
  tTextVersion.mColor = tTextVersionColor;
  tTextVersion.mAlign = tTextVersion.kAlignCenter;
  pGraphics->AttachControl(new ITextControl(this, tTextVersionIrect, &tTextVersion, (const char*)&sDisplayedVersion));

  // Clickable area leading to a website
  tWebsiteLink = new IURLControl(this, tWebsiteLinkIRect, PLUG_WEBSITE_LINK);
  pGraphics->AttachControl(tWebsiteLink);

  // Bugreport link
  static IText tBugreportLabel = IText(DLPG_BUGREPORT_LABEL_STRING_SIZE);
  tBugreportLabel.mColor = tBugreportLabelColor;
  tBugreportLabel.mSize = DLPG_BUGREPORT_LABEL_FONT_SIZE;
  tBugreportLabel.mAlign = tBugreportLabel.kAlignNear;
  pGraphics->AttachControl(
    new ITextControl(
      this,
      tBugreportLabelIrect,
      &tBugreportLabel,
      DLPG_BUGREPORT_LABEL_TEXT
      )
    );

  // Clickable area for bugreports
  MakeFeedbackUrl(sFeedbackUrl);
  tFeedbackLink = new IURLControl(this, tFeedbackLinkIRect, sFeedbackUrl);
  pGraphics->AttachControl(tFeedbackLink);

  // Post-init stuff
  AttachGraphics(pGraphics);
  setSampleRate(CONVERT_SAMPLE_RATE(GetSampleRate()));

  // Presets
  CreatePresets();

  // Low pass filter instance
  pmFilter = new Filter();
  pmFilter->setCutoff(CONVERT_LPF_FREQUENCY(FILTER_KNOB_DFT));
}

Transcriber::~Transcriber() {
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

  // On/off switch is off - change nothing
  // @todo Is there a cleaner way for bypassing a plugin?
  if (!mOnOff) {
    for (int i = 0; i < nFrames; ++i) {
      leftOutput[i] = leftInput[i];
      rightOutput[i] = rightInput[i];
    }
    return;
  }
  else {
    for (int i = 0; i < nFrames; ++i) {
      // Side channel
      fCurrentSample = leftInput[i] - rightInput[i];
      // Apply gain
      fCurrentSample *= mGain;
      // Output
      leftOutput[i] = rightOutput[i] = pmFilter->process(fCurrentSample);
    }
  }
}

void Transcriber::CreatePresets() {
  MakePreset("Default", 99., 0., true);
}

void Transcriber::Reset()
{
  TRACE;
  IMutexLock lock(this);
  setSampleRate(CONVERT_SAMPLE_RATE(GetSampleRate()));
  // @todo Tie the filter to sample rate properly

  // Update sample rate in feedback link
  MakeFeedbackUrl(sFeedbackUrl);
  tFeedbackLink = new IURLControl(this, tFeedbackLinkIRect, sFeedbackUrl);
  GetGUI()->AttachControl(tFeedbackLink);
}

void Transcriber::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kCutOffFrequency:
      pmFilter->setCutoff(CONVERT_LPF_FREQUENCY(GetParam(kCutOffFrequency)->Value()));
      break;

    case kSwitch:
      mOnOff = GetParam(kSwitch)->Value();
      ptCutOffFrequencyControl->GrayOut(!mOnOff);
      ptGainControl->GrayOut(!mOnOff);
      break;

    case kGain:
      mGain = LOG_TO_LINEAR(GetParam(kGain)->Value());
      break;

    default:
      break;
  }
}
