//
//  ZeroPoleChart.cpp
//  stepTest
//
//  Created by Luke Zeitlin on 09/08/2017.
//
//

#include "ZeroPoleChart.hpp"

ZPChart::ZPChart(IPlugBase *pPlug, IRECT pR, int paramIdx, const IColor* linecolor) : IControl(pPlug, pR, paramIdx)
{
    mlineColor = *linecolor;
    mWidth = (float)mDrawRECT.R - (float)mDrawRECT.L;
    mHight = (float)mDrawRECT.B - (float)mDrawRECT.T;
    mIsModeFB = true;
    mNumLines = 0;
    calcFreqs();
}

void ZPChart::setGain(double gain)
{
    mGain = gain;
    calcFreqs();
}

void ZPChart::setDelay(double delay)
{
    mDelay = delay;
    calcFreqs();
}

void ZPChart::calcFreqs()
{
    lineFreqs.clear();
    lineHights.clear();
    if(mIsModeFB)
    {
        for(int k = 0; k <= mDelay; k+=32)
        {
            lineFreqs.push_back(((float)k*mWidth) / mDelay);
            lineHights.push_back(-mGain);
        }
    }
    else
    {
        for(int k = 1; k <= mDelay; k+=32)
        {
            lineFreqs.push_back(((float)k*mWidth) / mDelay);
            lineHights.push_back(+mGain);
        }
    }
    SetDirty();
}
void ZPChart::setMode(bool isFeedBack)
{
    mIsModeFB = isFeedBack;
    calcFreqs();
    SetDirty();
}

bool ZPChart::Draw(IGraphics *pGraphics)
{
    double centerHight = mDrawRECT.B - int(mHight/2);
    for(int i = 0; i < lineFreqs.size(); ++i)
    {
        pGraphics->DrawVerticalLine(&mlineColor, mDrawRECT.L + int(lineFreqs[i]), centerHight, centerHight + mHight*( lineHights[i]/2));
        pGraphics->DrawVerticalLine(&mlineColor, mDrawRECT.L + 1 + int(lineFreqs[i]), centerHight, centerHight + mHight*( lineHights[i]/2));
    }
    return true;
}
