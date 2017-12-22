//
//  Point.hpp
//  MultiStep
//
//  Created by Luke Zeitlin on 15/08/2017.
//
//

#ifndef Point_hpp
#define Point_hpp

#include <stdio.h>

#endif /* Point_hpp */

class LZPoint
{
private:
    float m_X;
    float m_Y;
    
public:
    LZPoint(float x = 0., float y = 0.) : m_X(x), m_Y(y) {};
    void setX(float x){m_X = x;}
    void setY(float y){m_Y = y;}
    void setXY(float x, float y){m_X = x; m_Y = y;}
    float getX(){return m_X;}
    float getY(){return m_Y;}
};
