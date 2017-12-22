class IPeakMeterVert : public IControl
{
public:
    
    IPeakMeterVert(IPlugBase* pPlug, IRECT pR)
    : IControl(pPlug, pR)
    {
        mColor = COLOR_BLACK;
    }
    
    ~IPeakMeterVert() {}
    
    bool Draw(IGraphics* pGraphics)
    {
        pGraphics->FillIRect(&COLOR_GREEN, &mDrawRECT);
        IRECT filledBit = IRECT(mDrawRECT.L, mDrawRECT.T, mDrawRECT.R , mDrawRECT.B - (mValue * mDrawRECT.H()));
        pGraphics->FillIRect(&mColor, &filledBit);
        return true;
    }
    
    bool IsDirty() { return true;}
    
protected:
    IColor mColor;
};

class IPeakMeterHoriz : public IPeakMeterVert
{
public:
    
    bool Draw(IGraphics* pGraphics)
    {
        pGraphics->FillIRect(&COLOR_BLUE, &mDrawRECT);
        IRECT filledBit = IRECT(mDrawRECT.L, mDrawRECT.T, mDrawRECT.L + (mValue * mDrawRECT.W() ) , mDrawRECT.B );
        pGraphics->FillIRect(&mColor, &filledBit);
        return true;
    }
};
