#include "CombOver2.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "ZeroPoleChart.hpp"



const int kDummyParamForMultislider = -6;
const int kDummyParamForZPC = -3;
const int kDummyParamForMSE = -4;
const double METER_ATTACK = 0.6, METER_DECAY = 0.05;


enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  kFeedX = 34,
  kFeedY = 15,
  kDelX = 34,
  kDelY = 50,
  kWetX = 34,
  kWetY = 85,
  kModeX = 70,
  kModeY = 123,
  kSCLX = 85,
  kSCLY = 245,
  
  kLFONumX = 67,
  kLFONumY = 386,
  kLFODenomX = 67,
  kLFODenomY = 436,
  
  
  //LFO switch
  
  kLFOSwitchX = 250,
  kLFOSwitchY = 321,
  
  
  //Mod Matrix
  kMM_02X = 612,
  kMM_02Y = 392,
  kMM_12X = 614,
  kMM_12Y = 430,
  kMM_03X = 695,
  kMM_03Y = 392,
  kMM_13X = 697,
  kMM_13Y = 430,
  
  //Stepper
  kNumStepsX = 437,
  kNumStepsY = 312,
  kStepperL = 126,
  kStepperT = 370,
  kStepperR = 426,
  kStepperB = 470,
  
  //Sidechain
  kMeterLS_X = 40,
  kMeterLS_Y = 195,
  kMeterLS_W = 8,
  kMeterLS_H = 60,
  kMeterRS_X = 50,
  kMeterRS_Y = 195,
  kMeterRS_W = 8,
  kMeterRS_H = 60,
  
  //Bitmap frames
  kKnobFrames = 60
};


//----------------------------------------------------------Main Stuff


CombOver2::CombOver2(IPlugInstanceInfo instanceInfo)
: m_wetLevelDecimal(0.), m_feedLevelDecimal(0.), m_delayLengthSamples(0.), m_TargetDelaySam(0.), m_modTargetDelaySam(0.), m_sampleCounter(0), 	m_maxSampleCounter(0), m_NextDelaySams(0.), FeedMode(false), m_LFO1modVal(0.),MMFeedLevelIndex(0), MMDelLengthIndex(0), mDelLenMem(0.), mLevMem(0.), mSCVal(0.), mPrevLS(0.), mPrevRS(0.), mMeterIdx_LS(0), mMeterIdx_RS(0),  mNumSliders(0), mNumPoints(0), mCount(0), mPrevCount(0), isModeStepper(true), m_numerator(4), m_denominator(4), IPLUG_CTOR(kNumParams, 0, kNumPrograms, instanceInfo)
{
  TRACE;
  
  
  //init stepper
  for (int i = 0; i < NUM_SLIDERS; ++i)
  {
    mSteps[i] = 0.5;
  }
  
  cBufferL.reset(GetSampleRate());
  cBufferR.reset(GetSampleRate());
  
  cBufferL.setInterpSamN(5);
  cBufferR.setInterpSamN(5);
  
  // max sample Counter Value is 16 quater notes (4 bars)
  m_maxSampleCounter = ((GetSampleRate() * 60) / GetTempo()) * 16;
  
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kWetPercent)->InitDouble("Wet", 50., 0., 100.0, 0.01, "%");
  GetParam(kFeedLevelPercent)->InitDouble("Feed Level", 50., 0., 100.0, 0.01, "%");
  GetParam(kDelayLengthMS)->InitDouble("Delay Length", 10., 0.1, 30.0, 0.01, "%");
  GetParam(kFeedMode)->InitBool("FeedBack/FeedForward", false);
  GetParam(kLFO1Numerator)->InitInt("LFO1 Numerator", 4, 1, 16);
  GetParam(kLFO1Denominator)->InitInt("LFO1 Denominator", 4, 1, 16);
  GetParam(kMM_02)->InitDouble("Mod factor", 0., 0., 1., 0.01, "");
  GetParam(kMM_12)->InitDouble("Mod factor", 0., 0., 800., 0.1, "");
  GetParam(kMM_03)->InitDouble("Mod factor", 0., 0., 1., 0.01, "");
  GetParam(kMM_13)->InitDouble("Mod factor", 0., 0., 800., 0.1, "");
  GetParam(kNumSliders)->InitInt("steps", 8, 1, 50);
  GetParam(kNumPoints)->InitInt("points", 8, 1, 18);
  
  GetParam(kLFOSwitch)->InitBool("switch", true);
  
  
  
  
  //Attach graphics
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  
  pGraphics->AttachBackground(BG_ID, BG_FN);
  
  IBitmap* knob = pGraphics->LoadPointerToBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap* feedSwitch = pGraphics->LoadPointerToBitmap(FBSW_ID, FBSW_FN, 2);
  IBitmap* circle = pGraphics->LoadPointerToBitmap(CIR_ID, CIR_FN, 1);
  IBitmap* knMeter = pGraphics->LoadPointerToBitmap(KNOBMETER_ID , KNOBMETER_FN, 50);
  IBitmap* rad = pGraphics->LoadPointerToBitmap(RAD_ID , RAD_FN, 2);
  IBitmap* LFOmode = pGraphics->LoadPointerToBitmap(LFOMODE_ID , LFOMODE_FN, 2);
  
  //attach knobs
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedX, kFeedY, kFeedLevelPercent, knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kWetX, kWetY, kWetPercent, knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDelX, kDelY, kDelayLengthMS, knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSCLX, kSCLY, NULL, knob));
  
  //attach LFOswitch
  pGraphics->AttachControl(new ISwitchControl(this, kLFOSwitchX, kLFOSwitchY, kLFOSwitch, LFOmode));

  
  
  //text desplays
  IColor textColor = IColor(255, 0, 255, 0);
  IText textProps4(24, &textColor, "Verdana", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);
  
  IText textProps5(15, &textColor, "Verdana", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);
  
  pGraphics->AttachControl(new ICaptionControl(this, DRAW_RECT(IRECT(kLFONumX + 7, kLFONumY + 7, kLFONumX + 40, kLFONumY + 40)), kLFO1Numerator, &textProps4));
  pGraphics->AttachControl(new IKnobMultiControl(this, kLFONumX, kLFONumY, kLFO1Numerator, circle));
  pGraphics->AttachControl(new ICaptionControl(this, DRAW_RECT(IRECT(kLFODenomX + 7, kLFODenomY + 7, kLFODenomX + 40, kLFODenomY + 40)), kLFO1Denominator, &textProps4));
  pGraphics->AttachControl(new IKnobMultiControl(this, kLFODenomX, kLFODenomY, kLFO1Denominator, circle));
  
  //sidechain
  mMeterIdx_LS = *pGraphics->AttachControl(new IPeakMeterVert(this, MakeIRect(kMeterLS)));
  mMeterIdx_RS = *pGraphics->AttachControl(new IPeakMeterVert(this, MakeIRect(kMeterRS)));
  
  SetInputBusLabel(0, "main input");
  SetInputBusLabel(1, "sc input");
  SetOutputBusLabel(0, "output");
  
  //stepper
  IColor stepBlockColor = IColor(100, 0, 255, 0);
  mMSlider = new MultiSliderControlV(this, IRECT(kStepperL, kStepperT, kStepperR, kStepperB), kDummyParamForMultislider, NUM_SLIDERS, 10, &COLOR_WHITE, &stepBlockColor, &COLOR_YELLOW);
  pGraphics->AttachControl(mMSlider);
  pGraphics->AttachControl(new IBitmapControl(this, 100, 100, knMeter));
  
  
//  pGraphics->AttachControl(new ICaptionControl(this, DRAW_RECT(IRECT(kNumStepsX + 7, kNumStepsY + 7, kNumStepsX + 40, kNumStepsY + 40)), kNumSliders, &textProps4));
//  pGraphics->AttachControl(new IKnobMultiControl(this, kNumStepsX, kNumStepsY, kNumSliders, circle));
//  
  numStepsControl = new IKnobMultiControl(this, kNumStepsX, kNumStepsY, kNumSliders, circle);
  
  numStepsCaption = new ICaptionControl(this, DRAW_RECT(IRECT(kNumStepsX + 7, kNumStepsY + 7, kNumStepsX + 40, kNumStepsY + 40)), kNumSliders, &textProps4);
  
  pGraphics->AttachControl(numStepsCaption);
  pGraphics->AttachControl(numStepsControl);
  
  
  //MSE
  
  multiStep = new MSE(this, GetYCAIRO(), IRECT(kStepperL, kStepperT, kStepperR - 3, kStepperB), kDummyParamForMSE);
  pGraphics->AttachControl(multiStep);
  
  numPointsControl = new IKnobMultiControl(this, kNumStepsX, kNumStepsY, kNumPoints, circle);
  
  numPointsCaption = new ICaptionControl(this, DRAW_RECT(IRECT(kNumStepsX + 7, kNumStepsY + 7, kNumStepsX + 40, kNumStepsY + 40)), kNumPoints, &textProps4);
  
  pGraphics->AttachControl(numPointsCaption);
  pGraphics->AttachControl(numPointsControl);
  
  
  
  //Mod Matrix Controls
  
  pGraphics->AttachControl(new ICaptionControl(this, DRAW_RECT(IRECT(kMM_02X + 16, kMM_02Y + 12, kMM_02X + 26, kMM_02Y + 20)), kMM_02, &textProps5));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMM_02X, kMM_02Y, kMM_02, circle));
  
  pGraphics->AttachControl(new ICaptionControl(this, DRAW_RECT(IRECT(kMM_12X + 16, kMM_12Y + 12, kMM_12X + 26, kMM_12Y + 20)), (kMM_12), &textProps5));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMM_12X, kMM_12Y, kMM_12, circle));
  
  pGraphics->AttachControl(new ICaptionControl(this, DRAW_RECT(IRECT(kMM_03X + 16, kMM_03Y + 12, kMM_03X + 26, kMM_03Y + 20)), kMM_03, &textProps5));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMM_03X, kMM_03Y, kMM_03, circle));
  
  pGraphics->AttachControl(new ICaptionControl(this, DRAW_RECT(IRECT(kMM_13X + 16, kMM_13Y + 12, kMM_13X + 26, kMM_13Y + 20)), (kMM_13), &textProps5));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMM_13X, kMM_13Y, kMM_13, circle));
  
  //Mode switch
  pGraphics->AttachControl(new ISwitchControl(this, kModeX, kModeY, kFeedMode, feedSwitch));
  
  //Zero-Pole Chart
  IColor linesCol = IColor(255, 0, 0, 0);
  ZP = new ZPChart(this, IRECT(250, 40, 750, 250), kDummyParamForZPC, &linesCol);
  
  pGraphics->AttachControl(ZP);
  
  // ALL CONTROL ATTACHING ABOVE THIS POINT
  AttachGraphics(pGraphics);
  
  //Mod Matrix. [destination]   [source]
  MMFeedLevelIndex = matrix.addDestination(); // index 0
  MMDelLengthIndex = matrix.addDestination(); // index 1
  
  matrix.addSource(&m_feedLevelDecimal);      // index 0
  matrix.addSource(&m_TargetDelaySam);        // index 1
  matrix.addSource(&m_LFO1modVal);            // index 2
  matrix.addSource(&mSCVal);                  // index 3
  
  matrix.buildMatrix();
  
  //Set non-controllable elements in the MM
  matrix.setValInMatrix(1., 0, 0);
  matrix.setValInMatrix(1., 1, 1);
  matrix.setValInMatrix(0., 1, 0);
  matrix.setValInMatrix(0., 0, 1);
  
  //Preset Stuff
  RestorePreset(0);
  
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

//
void CombOver2::smoothDelay()
{
  double A = 0.1;
  double targetDel = *(matrix.getDestinationValAddress(MMDelLengthIndex));
  double maxSamps = (GetSampleRate() / 1000.) * 30.;
  targetDel = std::max(1.0, std::min(targetDel, maxSamps));
  m_delayLengthSamples = (1.0-A)*m_delayLengthSamples+A* targetDel;
  cBufferL.setDelayLength(m_delayLengthSamples);
  cBufferR.setDelayLength(m_delayLengthSamples);
}



CombOver2::~CombOver2() {}

void CombOver2::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  
  if(m_delayLengthSamples != mDelLenMem)
  {
    ZP->setDelay(m_delayLengthSamples);
    mDelLenMem = m_delayLengthSamples;
  }
  double feedlevel = min(*matrix.getDestinationValAddress(0), 1.);
  if(feedlevel != mLevMem)
  {
    ZP->setGain(feedlevel);
    mLevMem = *matrix.getDestinationValAddress(0);
  }
  
  // Mutex is already locked for us.

  
  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* scin1 = inputs[2];
  double* scin2 = inputs[3];
  
  double* out1 = outputs[0];
  double* out2 = outputs[1];
  
  double peakLS = 0.0, peakRS = 0.0;
  
  ITimeInfo ti;
  GetTime(&ti);
  matrix.calcMatrix();
  
  m_LFO1modVal = 0.;
  
  
  if(!ti.mTransportIsRunning)
  {
    if(isModeStepper)
    {
    m_LFO1modVal = LFO1.calcPhaseAndRead(((GetSampleRate() * 60.) / GetTempo()) * 16., m_sampleCounter);
    }
    else
    {
      m_LFO1modVal = multiStep->getDataFromPath(calcPhaseMSE());
    }
  }
  if(ti.mTransportIsRunning && ti.mPPQPos != -1)
  {
    if(isModeStepper)
    {
       m_LFO1modVal = LFO1.calcPhaseAndRead(ti.mPPQPos);
    }
    else
    {
      m_LFO1modVal = multiStep->getDataFromPath(calsPhaseMSETrans(ti.mPPQPos));
    }
  }
  
  if(isModeStepper)
  {
    mMSlider->SetHighlight(ceil( double(mNumSliders) * LFO1.percentPhase() / 100. ) - 1);
  }
  
  smoothDelay();
  
  double wet = wetSmooth.Process(m_wetLevelDecimal);
  std::cout<<wet<<std::endl;
  

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    
    
    
    if(FeedMode == MODE_FEEDBACK)
    {
      *out1 = (*in1 - (cBufferL.getRead()* feedlevel));
      *out2 = (*in2 - (cBufferR.getRead()* feedlevel));
      cBufferL.writeToBuffer(*in1);
      cBufferR.writeToBuffer(*in2);
    }
    else if(FeedMode == MODE_FEEDFORWARD)
    {
      *out1 = cBufferL.getRead();
      *out2 = cBufferR.getRead();
      double sumL = *in1 - (cBufferL.getRead() * feedlevel);
      double sumR = *in1 - (cBufferR.getRead() * feedlevel);
      cBufferL.writeToBuffer(sumL);
      cBufferR.writeToBuffer(sumR);
    }
    
    *out1 = (*out1 * wet) + (*in1 * 1.0 - wet);
    *out2 = (*out2 * wet) + (*in2 * 1.0 - wet);
    
    
    //--------------------------------------------------------------------
//    if(sizeof(inputs) > 2)
//    {
//    peakLS = IPMAX(peakLS, fabs(*scin1));
//    peakRS = IPMAX(peakRS, fabs(*scin2));
//    }
//Sidechain switched off for App mode. When building for AU use the above
    //--------------------------------------------------------------------

      peakLS = IPMAX(peakLS, fabs(*in1));
      peakRS = IPMAX(peakRS, fabs(*in2));

    //--------------------------------------------------------------------
    ++m_sampleCounter;
  }
  double xLS = (peakLS < mPrevLS ? METER_DECAY : METER_ATTACK);
  double xRS = (peakRS < mPrevRS ? METER_DECAY : METER_ATTACK);
  
  peakLS = peakLS * xLS + mPrevLS * (1.0 - xLS);
  peakRS = peakRS * xRS + mPrevRS * (1.0 - xRS);
  
  mPrevLS = peakLS;
  mPrevRS = peakRS;
  
  mSCVal = (mPrevLS + mPrevRS) / 2.;
  {
    GetGUI()->SetControlFromPlug(mMeterIdx_LS, peakLS);
    GetGUI()->SetControlFromPlug(mMeterIdx_RS, peakRS);
  }
}

void CombOver2::Reset()
{
  TRACE;
  IMutexLock lock(this);
  cBufferL.reset(GetSampleRate());
  cBufferR.reset(GetSampleRate());
  mCount = 0;
  mPrevCount = ULONG_MAX;

}

void CombOver2::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  switch (paramIdx)
  {
      break;
    case kWetPercent:
      m_wetLevelDecimal = GetParam(kWetPercent)->Value() / 100.;
      break;
    case kFeedLevelPercent:
      m_feedLevelDecimal = GetParam(kFeedLevelPercent)->Value() / 100.;
      break;
    case kFeedMode:
      FeedMode = GetParam(kFeedMode)->Value();
      ZP->setMode(FeedMode);
      break;
    case kDelayLengthMS:
      m_TargetDelaySam = GetParam(kDelayLengthMS)->Value()*GetSampleRate() / 1000.;
      break;
    case kLFO1Numerator:
      LFO1.setRate(GetParam(kLFO1Numerator)->Value(), GetParam(kLFO1Denominator)->Value());
      m_numerator = GetParam(kLFO1Numerator)->Value();
      break;
    case kLFO1Denominator:
      LFO1.setRate(GetParam(kLFO1Numerator)->Value(), GetParam(kLFO1Denominator)->Value());
      m_numerator = GetParam(kLFO1Denominator)->Value();
      break;
    case kMM_02:
      matrix.setValInMatrix(GetParam(kMM_02)->Value(), 0, 2);
      break;
    case kMM_12:
      matrix.setValInMatrix(GetParam(kMM_12)->Value(), 1, 2);
      break;
    case kMM_03:
      matrix.setValInMatrix(GetParam(kMM_03)->Value(), 0, 3);
      break;
    case kMM_13:
      matrix.setValInMatrix(GetParam(kMM_13)->Value(), 1, 3);
      break;
      
    case kDummyParamForMultislider:
      mMSlider->GetLatestChange(mSteps);
      LFO1.writeFromStepper(mSteps, mNumSliders);
      break;
      
    case kDummyParamForMSE:
      break;
      
      
    case kNumSliders:
      mNumSliders = GetParam(kNumSliders)->Value();
      mMSlider->setStepsDisplayed(mNumSliders);
      LFO1.writeFromStepper(mSteps, mNumSliders);
      break;
      
    case kNumPoints:
    {
      int temp = GetParam(kNumPoints)->Value();
      if (temp != mNumPoints)
      {
        mNumPoints = GetParam(kNumPoints)->Value();
        multiStep->setNumPoints(mNumPoints);
      }
    
      break;
    }
      
    case kLFOSwitch:
    {
      isModeStepper = GetParam(kLFOSwitch)->Value();
      switchLFOMode();
      break;
    }
      
    default:
    {
      break;
    }
  }
}

void CombOver2::switchLFOMode()
{
  if(isModeStepper)
  {
    mMSlider->Hide(false);
    multiStep->Hide(true);
    numStepsCaption->Hide(false);
    numStepsControl->Hide(false);
    numPointsCaption->Hide(true);
    numPointsControl->Hide(true);
  }
  else
  {
    mMSlider->Hide(true);
    multiStep->Hide(false);
    numStepsCaption->Hide(true);
    numStepsControl->Hide(true);
    numPointsCaption->Hide(false);
    numPointsControl->Hide(false);
//    multiStep->setPoints(mPointsV, mContPointsV);
  }
}

double CombOver2::calcPhaseMSE()
{
  double numSamps4Bars = ((GetSampleRate() * 60.) / GetTempo()) * 16.;
  int currentSample = m_sampleCounter;
  int sampsPerCycle = (int)((numSamps4Bars / 4.) / (double)m_denominator)*(double)m_numerator;
  int adjustedCurrentSample = currentSample % sampsPerCycle;
  double decimalPhase = ((double)adjustedCurrentSample / (double)sampsPerCycle);
  return(decimalPhase);

}

double CombOver2::calsPhaseMSETrans(double q)
{
    double Q = (4.*m_numerator) / m_denominator;
    double i = fmod(q, Q) / Q;
    return(i);
}



//----------------------------------------------------------Methods for stepper


// this over-ridden method is called when the host is trying to store the plug-in state and needs to get the current data from your algorithm
bool CombOver2::SerializeState(ByteChunk* pChunk)
{
  TRACE;
  IMutexLock lock(this);
  double v;
  double x;
  double y;
  
  mPointsV = multiStep->getPointsVector();
  mContPointsV = multiStep->getContPointsVector();
  
  // serialize the multi-slider state state before serializing the regular params
  for (int i = 0; i< NUM_SLIDERS; i++)
  {
    v = mSteps[i];
    pChunk->Put(&v);
  }
  v = mNumPoints;
  pChunk->Put(&v);
  
  for (int i = 0; i< mNumPoints; i++)
  {
    x = mPointsV[i].getX();
    pChunk->Put(&x);
    y = mPointsV[i].getY();
    pChunk->Put(&y);
  }
  
  for (int i = 0; i< mNumPoints - 1; i++)
  {
    x = mContPointsV[i].getX();
    pChunk->Put(&x);
    y = mContPointsV[i].getY();
    pChunk->Put(&y);
  }
  
  
  return IPlugBase::SerializeParams(pChunk); // must remember to call SerializeParams at the end
}

// this over-ridden method is called when the host is trying to load the plug-in state and you need to unpack the data into your algorithm
int CombOver2::UnserializeState(ByteChunk* pChunk, int startPos)
{
  TRACE;
  IMutexLock lock(this);
  double v = 0.0;
  double x = 0.0;
  double y = 0.0;

  
  // unserialize the multi-slider state before unserializing the regular params
  
  
  for (int i = 0; i< NUM_SLIDERS; i++)
  {
    v = 0.0;
    startPos = pChunk->Get(&v, startPos);
    mSteps[i] = v;
  }
  v = 0.0;
  startPos = pChunk->Get(&v, startPos);
  int numPoints = v;
  
  std::vector<LZPoint> tempVec;
  std::vector<LZPoint> tempCVec;
  
  
  for (int i = 0; i< numPoints; i++)
  {
    x = 0.0;
    y = 0.0;
    startPos = pChunk->Get(&x, startPos);
    startPos = pChunk->Get(&y, startPos);
    tempVec.push_back(LZPoint(x, y));
  }
  
  for (int i = 0; i< numPoints - 1; i++)
  {
    x = 0.0;
    y = 0.0;
    startPos = pChunk->Get(&x, startPos);
    startPos = pChunk->Get(&y, startPos);
    tempCVec.push_back(LZPoint(x, y));
  }
  
  
  // update values in control, will set dirty
  if(mMSlider)
    mMSlider->SetState(mSteps);
  multiStep->setPoints(numPoints, tempVec, tempCVec);
  mNumPoints = numPoints;
  mPointsV = tempVec;
  mContPointsV = tempCVec;
  
  
  
  return IPlugBase::UnserializeParams(pChunk, startPos); // must remember to call UnserializeParams at the end
}

bool CombOver2::CompareState(const unsigned char* incomingState, int startPos)
{
  bool isEqual = true;
  const double* data = (const double*) incomingState;
  startPos = NUM_SLIDERS * sizeof(double);
  isEqual = (memcmp(data, mSteps, startPos) == 0);
  isEqual &= IPlugBase::CompareState(incomingState, startPos); // fuzzy compare regular params
  
  return isEqual;
}

void CombOver2::PresetsChangedByHost()
{
  TRACE;
  IMutexLock lock(this);
  
  if(mMSlider)
    mMSlider->SetState(mSteps);
  
  if(GetGUI())
    GetGUI()->SetAllControlsDirty();
}



