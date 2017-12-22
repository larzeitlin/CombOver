//
//  Drawing.hpp
//  LPF
//
//  Created by Luke Zeitlin on 17/06/2017.
//
//

#ifndef Drawing_hpp
#define Drawing_hpp

#include <stdio.h>
#include "ycairo.h"
#include "resource.h"

#include <vector>
#include "IControl.h"
#include "Point.hpp"

class MSE : public IControl, ycairo_gui
{
private:

    std::vector<LZPoint> pointsVector;
    std::vector<LZPoint> controlPoints;
    float m_numPoints = 8;
    int m_pointSelected = -1;
    int m_controlPointSelected = -1;
    float xMoveLMax;
    float xMoveRMax;
    float yMoveUMax;
    float yMoveDMax;
    
    
    LZPoint marker;
//    LZPoint centerPointMarker1;
//    LZPoint centerPointMarker2;
    
public:
    
    
    MSE(IPlugBase* pPlug, ycairo_base *ycairo_base, IRECT pR, int paramIdx)
    : IControl(pPlug, pR, paramIdx), ycairo_gui(ycairo_base, this) {setNumPoints(m_numPoints);}
    ~MSE();
    
    
    void OnMouseDown(int x, int y, IMouseMod *pMod);
    void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod *pMod);
    void OnMouseUp(int x, int y, IMouseMod *pMod);
    
    bool isDirty() {return true; };
    bool Draw(IGraphics *pGraphics);
    
    int mouseX = 0;
    int mouseY = 0;
    
    cairo_path_t *MSEpath;
    
    void setNumPoints(int numPoints);
    
    double getDataFromPath(double decimal);
    
    float  return_x_Bezier_t(float t, LZPoint p0, LZPoint p1, LZPoint p2, LZPoint p3);
    
    float  return_y_Bezier_t(float t, LZPoint p0, LZPoint p1, LZPoint p2, LZPoint p3);
    
    int RectRelX(int x);
    int RectRelY(int y);
    
    std::vector<LZPoint> getPointsVector();
    std::vector<LZPoint> getContPointsVector();
    
    void setPoints(int size, std::vector<LZPoint> &pointsV, std::vector<LZPoint> &CpointsV );
    
    
};

#endif /* Drawing_hpp */
