#ifndef _IVECSLIDERS_
#define _IVECSLIDERS_

#ifdef WIN32
#define round(x) floor(x + 0.5)
#endif
#include <vector>

#define MAXSLIDERS 50

class MultiSliderControlV: public IControl
{
public:
  MultiSliderControlV(IPlugBase *pPlug,
                      IRECT pR,
                      int paramIdx,
                      int numSliders,
                      int handleWidth,
                      const IColor* bgcolor,
                      const IColor* fgcolor,
                      const IColor* hlcolor)
    : IControl(pPlug, pR, paramIdx)
  {
    mBgColor = *bgcolor;
    mFgColor = *fgcolor;
    mHlColor = *hlcolor;
    mNumSliders = numSliders;
    mHighlighted = -1;
    mGrain = 0.001;
    mSliderThatChanged = -1;
    mStepsDisplayed = 8;

    mSteps = new double[numSliders];

    for(int i=0; i<numSliders; i++)
    {
        mSteps[i] = 0.;
    }

    mHandleWidth = handleWidth;
  }

  ~MultiSliderControlV()
  {
    delete [] mSteps;
      
  }

  bool Draw(IGraphics* pGraphics)
  {
      m_SliderBounds.clear();
      
      float sliderWidth = floor((float) mDrawRECT.W() / (float) mStepsDisplayed);
      for(int i=0; i<mStepsDisplayed; i++)
      {
          int lpos = (i * sliderWidth);
          m_SliderBounds.push_back(IRECT(mDrawRECT.L + lpos , mDrawRECT.T, mDrawRECT.L + lpos + sliderWidth, mDrawRECT.B - 5));
          
      float yPos = mSteps[i] * mDrawRECT.H();
      int top = mDrawRECT.B - yPos;
      int bottom = top + 4;

      IColor * color = &mFgColor;
      if(i == mHighlighted) color = &mHlColor;

      IRECT srect = IRECT(m_SliderBounds[i].L, top, m_SliderBounds[i].R-1, bottom);
      pGraphics->FillIRect(color, &srect );
    }
    return true;
  }

  void OnMouseDown(int x, int y, IMouseMod* pMod)
  {
    SnapToMouse(x, y);
  }

  void OnMouseUp(int x, int y, IMouseMod* pMod)
  {
    //TODO: check this isn't going to cause problems... this will happen from the gui thread
    mPlug->ModifyCurrentPreset();
    mPlug->DirtyPTCompareState();
  }

  void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod)
  {
    SnapToMouse(x, y);
  }

  void SnapToMouse(int x, int y)
  {
      x = BOUNDED(x, mDrawRECT.L, m_SliderBounds[mStepsDisplayed-1].R-1);
    y = BOUNDED(y, mDrawRECT.T, mDrawRECT.B-6);

    float yValue =  (float) (y-mDrawRECT.T) / (float) mDrawRECT.H();

    yValue = round( yValue / mGrain ) * mGrain;

    int sliderTest = mStepsDisplayed-1;
    bool foundIntersection = false;

    while (!foundIntersection)
    {
      foundIntersection = m_SliderBounds[sliderTest].Contains(x, y);
      if (!foundIntersection && sliderTest !=0 ) sliderTest--;
    }

    if (foundIntersection)
    {
      mHighlighted = sliderTest;
      mSteps[sliderTest] = 1. - BOUNDED(yValue, 0., 1.);
      mSliderThatChanged = sliderTest;
      mPlug->OnParamChange(mParamIdx); // TODO: rethink this WRT threading
    }
    else
    {
      mSliderThatChanged = -1;
      mHighlighted = -1;
    }
    SetDirty();
  }

  void GetLatestChange(double* data)
  {
    data[mSliderThatChanged] = mSteps[mSliderThatChanged];
  }

  void GetState(double* data)
  {
    memcpy( data, mSteps, mNumSliders * sizeof(double));
  }

  void SetState(double* data)
  {
    memcpy(mSteps, data, mNumSliders * sizeof(double));
    SetDirty();
  }

  void SetHighlight(int i)
  {
    mHighlighted = i;
    SetDirty();
  }
    
    void setStepsDisplayed(int numSteps)
    {
        mStepsDisplayed = numSteps;
        SetDirty();
    }

private:
    IColor mBgColor, mFgColor, mHlColor;
    int mNumSliders;
    int mHandleWidth;
    int mSliderThatChanged;
    double *mSteps;
    double mGrain;
    std::vector<IRECT>m_SliderBounds;
    int mHighlighted;
    int mStepsDisplayed;
};

#endif _IVECSLIDERS_
