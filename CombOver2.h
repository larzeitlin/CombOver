#ifndef __CombOver2__
#define __CombOver2__

#include "IPlug_include_in_plug_hdr.h"
#include "CBuffer.hpp"
#include "LFO.hpp"
#include "ModMatrix.hpp"
#include "IPlugChunks_controls.h"
#include "ZeroPoleChart.hpp"
#include "IPlugSideChain_Controls.h"
#include "IMidiQueue.h"
#include "MSE.hpp"
#include <vector>
#include "smooth.hpp"

#define NUM_SLIDERS 50
#define NUM_POINTS_CHUNKS 19


enum feedMode: bool
{
  MODE_FEEDBACK = false,
  MODE_FEEDFORWARD = true
  };
  
  
  class CombOver2 : public IPlug
  {
  public:
    CombOver2(IPlugInstanceInfo instanceInfo);
    ~CombOver2();
    void Reset();
    void OnParamChange(int paramIdx);
    void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
    void smoothDelay();
    void modDelayLength();
    void switchLFOMode();
    double calcPhaseMSE();
    double calsPhaseMSETrans(double q);
    
    //for stepper:
    bool SerializeState(ByteChunk* pChunk);
    int UnserializeState(ByteChunk* pChunk, int startPos);
    bool CompareState(const unsigned char* incomingState, int startPos);
    void PresetsChangedByHost();
    
  private:
    double mGain = 1.0;
    
    //member params:
    double m_wetLevelDecimal;
    double m_feedLevelDecimal;
    double m_delayLengthSamples;
    double m_TargetDelaySam;
    double m_modTargetDelaySam;
    int m_sampleCounter;
    int m_maxSampleCounter;
    double m_NextDelaySams;
    bool FeedMode;
    double m_LFO1modVal;
    int MMFeedLevelIndex;
    int MMDelLengthIndex;
    double mDelLenMem;
    double mLevMem;
    double mSCVal;
    bool isStepperOn;
    bool isModeStepper;
    int m_numerator;
    int m_denominator;
    
    //smoother
    CParamSmooth wetSmooth;
    
    
    //member objects:
    Cbuffer cBufferL;
    Cbuffer cBufferR;
    ITimeInfo ti;
    ModMatrix matrix;
    LFO LFO1;
    ZPChart* ZP;
    
    //for stepper:
    double mSteps[NUM_SLIDERS];
    int mNumSliders;
    unsigned long mCount, mPrevCount;
    IGraphics* pGraphics;
    MultiSliderControlV *mMSlider;
    ICaptionControl *numStepsCaption;
    IKnobMultiControl *numStepsControl;
    
    //for MSE:
    MSE *multiStep;
    int mNumPoints;
    ICaptionControl *numPointsCaption;
    IKnobMultiControl *numPointsControl;
    std::vector<LZPoint> mPointsV;
    std::vector<LZPoint> mContPointsV;
    bool initFlag = false;
    
    
    
    
    //temp tests
    double markerx = 0.;
    
    
    
    //sidechain
    double mPrevLS;
    double mPrevRS;
    int mMeterIdx_LS;
    int mMeterIdx_RS;
    
    
  };
  
#endif
