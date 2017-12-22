#include "MSE.hpp"
#include "iostream"
#include <stdlib.h>


MSE::~MSE()
{
    cairo_path_destroy(MSEpath);
}

bool MSE::Draw(IGraphics *pGraphics)
{

    ycairo_prepare_draw();
    
    cairo_set_source_rgb(cr, 0, 255, 0);
    
    cairo_set_line_width(cr, 3);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    
    
    for (int i = 0; i <= m_numPoints; ++i)
    {

        ycairo_circle(cr, pointsVector[i].getX(), pointsVector[i].getY(), 5);
        cairo_fill(cr);
    }
    
    
    cairo_move_to(cr, pointsVector[0].getX(), pointsVector[0].getY());
    
    for (int i = 1; i <= m_numPoints; ++i)
    {
        cairo_curve_to(cr, controlPoints[i -1].getX(), controlPoints[i - 1].getY(), controlPoints[i - 1].getX(), controlPoints[i - 1].getY(), pointsVector[i].getX(), pointsVector[i].getY());
        
    }
    cairo_stroke(cr);
    
    MSEpath = cairo_copy_path(cr);
    
    cairo_close_path(cr);
    
    for (int i = 0; i < m_numPoints; ++i)
    {
        
        ycairo_circle(cr, controlPoints[i].getX(), controlPoints[i].getY(), 3);
        cairo_set_source_rgba(cr, 0, 255, 255, 150);
        cairo_fill(cr);
    }
    
    ycairo_circle(cr, marker.getX(), marker.getY(), 3);
    cairo_set_source_rgba(cr, 255, 255, 0, 150);
    cairo_fill(cr);
    
    ycairo_draw();
    
    return true;
}


void MSE::OnMouseDown(int x, int y, IMouseMod *pMod)
{
    
    mouseX = x;
    mouseY = y;
    
    
    for(int i = 0; i <= m_numPoints; ++i)
    {
        
        if(abs(mouseX - pointsVector[i].getX()) < 10 && abs(mouseY - pointsVector[i].getY()) < 10)
        {
            m_pointSelected = i;
            xMoveLMax = pointsVector[i - 1].getX() + 1;
            xMoveRMax = pointsVector[i + 1].getX() - 1;
        }
        
        else if(abs(mouseX - controlPoints[i].getX()) < 10 && abs(mouseY - controlPoints[i].getY()) < 10)
        {
            m_controlPointSelected = i;
            xMoveLMax = pointsVector[i].getX() + 1;
            xMoveRMax = pointsVector[i + 1].getX() - 1;
            
            yMoveUMax = min(pointsVector[i].getY() + 1,pointsVector[i + 1].getY() + 1 );
            yMoveDMax = max(pointsVector[i + 1].getY() - 1,pointsVector[i].getY() - 1) ;
        }
    }
    
    SetDirty();
}

void MSE::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod *pMod)
{
    
    if(m_pointSelected  != -1)
    {
        float boundX = min(max((float)x, xMoveLMax), xMoveRMax);
        float boundY = min(max(float(y), (float)mDrawRECT.T), (float)mDrawRECT.B);
        if(m_pointSelected == 0 or m_pointSelected == m_numPoints)
        {
            boundX = pointsVector[m_pointSelected].getX();
        }
        pointsVector[m_pointSelected].setXY(boundX, boundY);
        
        controlPoints[m_pointSelected].setX(boundX + ((pointsVector[m_pointSelected + 1].getX() - boundX) / 2. ));
        controlPoints[m_pointSelected].setY(boundY + ((pointsVector[m_pointSelected + 1].getY() - boundY) / 2. ));
        
        controlPoints[m_pointSelected - 1].setX(pointsVector[m_pointSelected - 1].getX() + ((boundX - pointsVector[m_pointSelected - 1].getX()) / 2. ));
        controlPoints[m_pointSelected - 1].setY(pointsVector[m_pointSelected - 1].getY() + ((boundY - pointsVector[m_pointSelected - 1].getY()) / 2. ));
    }
    
    
    else if(m_controlPointSelected  != -1)
    {
        float boundX = min(max((float)x, xMoveLMax), xMoveRMax);
        float boundY = min(max((float)y, yMoveUMax), yMoveDMax);
        controlPoints[m_controlPointSelected].setXY(boundX, boundY);
    }
    
    
    SetDirty();
}



void MSE::setNumPoints(int numPoints)
{
    m_numPoints = numPoints;
    pointsVector.clear();
    controlPoints.clear();
    float widthBetweenPoints = mDrawRECT.W() / m_numPoints;
    for (int i = 0; i <= m_numPoints; ++i)
    {
        pointsVector.push_back(LZPoint( mDrawRECT.L + widthBetweenPoints * i, mDrawRECT.T + (float)(mDrawRECT.H() / 2)));
    }
    
    for (int i = 0; i < m_numPoints; ++i)
    {
        controlPoints.push_back(LZPoint( mDrawRECT.L + (widthBetweenPoints / 2.) +  widthBetweenPoints * i, mDrawRECT.T  + (float)(mDrawRECT.H() / 2)));
    }
    

}

void MSE::OnMouseUp(int x, int y, IMouseMod *pMod)
{
    m_controlPointSelected = -1;
    m_pointSelected = -1;
    //TODO: check this isn't going to cause problems... this will happen from the gui thread
    mPlug->ModifyCurrentPreset();
    mPlug->DirtyPTCompareState();
    

}

//takes a number between 0.0 and 1.0
double MSE::getDataFromPath(double decimal)
{
    
    //find the section of the MSE that the point is in
    int section;
    for(int i = 0; i < sizeof(pointsVector); ++i)
    {
        double sectionDecimal =  (pointsVector[i].getX()  - mDrawRECT.L ) /  mDrawRECT.W();
        if (sectionDecimal == decimal) {return(pointsVector[i].getY() / mDrawRECT.H());}
        if (sectionDecimal < decimal) {section = i;}
        if (sectionDecimal  > decimal) {break;}
    }
    
    float relitive_x = mDrawRECT.L + (decimal * mDrawRECT.W());
    
    float t_guess = 0.5;
    float t_max = 1.0;
    float t_min = 0.0;
    bool isSolved = false;
    

    while(! isSolved)
    {
        
        float x_try = return_x_Bezier_t(t_guess, pointsVector[section], controlPoints[section], controlPoints[section], pointsVector[section + 1]);
        if(abs(relitive_x - x_try) < 1.0) { isSolved = true; }
        else if (relitive_x > x_try) {t_min = t_guess; t_guess = ((t_max - t_min) / 2.) + t_min;}
        else if (relitive_x < x_try) {t_max = t_guess; t_guess = t_max - ((t_max - t_min)/2.);}
        else { std::cerr << "An error occurred in the logic here" << std::endl; }
    }
    
    
    
    float y = return_y_Bezier_t(t_guess, pointsVector[section], controlPoints[section], controlPoints[section], pointsVector[section + 1]);
    marker.setXY(relitive_x, y);
    
    
    SetDirty();
    return((y - mDrawRECT.T) / mDrawRECT.H());
}

float MSE::return_x_Bezier_t(float t, LZPoint p0, LZPoint p1, LZPoint p2, LZPoint p3)
{
    return (
    
           pow(1.- t, 3) * p0.getX() + 3 * pow(1.- t, 2) * t * p1.getX() + 3. * (1. - t ) * pow(t, 2.) * p2.getX() + pow(t, 3.) * p3.getX()
    
           );
}

float MSE::return_y_Bezier_t(float t, LZPoint p0, LZPoint p1, LZPoint p2, LZPoint p3)
{
    return (
            
            pow(1.- t, 3) * p0.getY() + 3 * pow(1.- t, 2) * t * p1.getY() + 3. * (1. - t ) * pow(t, 2.) * p2.getY() + pow(t, 3.) * p3.getY()
            
            );
}

std::vector<LZPoint> MSE::getPointsVector()
{
    return(pointsVector);
}

std::vector<LZPoint> MSE::getContPointsVector()
{
    return(controlPoints);
}

void MSE::setPoints(int size, std::vector<LZPoint> &pointsV, std::vector<LZPoint> &CpointsV)
{
    setNumPoints(size);
    pointsVector = pointsV;
    controlPoints = CpointsV;
    SetDirty();
}

