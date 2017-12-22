#ifndef ZeroPoleChart_hpp
#define ZeroPoleChart_hpp

#include <stdio.h>
#include "IControl.h"
#include <vector>


class ZPChart: public IControl
{
public:
    ZPChart(IPlugBase *pPlug, IRECT pR, int paramIdx, const IColor* linecolor);
    void setGain(double gain);
    void setDelay(double delay);
    void setMode(bool isFeedBack);
    void calcFreqs();
    bool Draw(IGraphics* pGraphics);
    std::vector<double> lineFreqs;
    std::vector<double> lineHights;
    
private:
    IColor mlineColor;
    int mNumLines;
    double mGain;
    bool mIsModeFB;
    double mDelay;
    double mWidth;
    double mHight;
};




#endif /* ZeroPoleChart_hpp */
