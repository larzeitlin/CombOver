#ifndef CBuffer_hpp
#define CBuffer_hpp

#include <stdio.h>

class Cbuffer
{
    
private:
    double m_delayLength;
    int writeIndex;
    int m_length;
    double* mpBuffer = NULL;
    int interpSampN;
    
public:
    Cbuffer(int length = 44100);
    ~Cbuffer();
    void reset(int length = 44100);           
    void setDelayLength(int delayLength);
    void writeToBuffer(double inVal);
    void setDelayLength(double newDelay);
    double getRead();
    double lagrangeInterp(double* dataPoints, int length, double x);
    void setInterpSamN(int numSams);
};


#endif /* CBuffer_hpp */
