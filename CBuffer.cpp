//
//  CBuffer.cpp
//  CombOver
//
//  Created by Luke Zeitlin on 27/07/2017.
//
//

#include "CBuffer.hpp"

#include <cstring>
#include <iostream>

Cbuffer::Cbuffer(int length): m_length(length)
{
    mpBuffer = new double[m_length];
    memset(mpBuffer, 0, length*sizeof(double));
    m_delayLength = 0.;
    writeIndex = 0;
    interpSampN = 5;
}


Cbuffer::~Cbuffer()
{
    delete [] mpBuffer;
}

void Cbuffer::reset(int length)
{
    m_length = length;
    mpBuffer = new double[m_length];
    memset(mpBuffer, 0, length*sizeof(double));
    m_delayLength = 0.;
    writeIndex = 0;
}

void Cbuffer::writeToBuffer(double inVal)
{
    mpBuffer[writeIndex] = inVal;
    ++writeIndex;
    if(writeIndex >= m_length)  { writeIndex = 0;}
}


void Cbuffer::setDelayLength(double delayLength)
{
    m_delayLength = delayLength;
}

double Cbuffer::lagrangeInterp(double* dataPoints, int length, double x)
{
    using namespace std;
    double sum = 0;
    int N = length - 1;
    
    for(double k = 0; k <= N; k += 1.)
    {
        double product = 1.0;
        for(double n = 0; n <= N; n += 1.)
        {
            if(k - n != 0)
            {
                product = product * ((x - n) / (k - n));
            }
        }
        sum += (product * dataPoints[(int)k]);
    }
    return(sum);
}

void Cbuffer::setInterpSamN(int numSams)
{
    interpSampN = numSams;
}


double Cbuffer::getRead()
{
    double globalTargetX = (double(writeIndex) - m_delayLength);
    
    if(globalTargetX < 0) { globalTargetX += (double)m_length;}
    int centerSampleX = (int)globalTargetX;
    double dataPoints [interpSampN];
    int startSamp = 0;
    double targetX = globalTargetX - (int)globalTargetX + (int)(interpSampN / 2);
    
    //for centering odd / even number of samples
    if(interpSampN % 2 == 1)    {startSamp = interpSampN / 2; }
    else                        {startSamp = (interpSampN - 1) / 2;}
    
    {
        for(int i = 0; i < interpSampN; ++i)
        {
            int k = centerSampleX - startSamp + i;
            while(k < 0) {k += m_length;}
            if(k >= m_length) {k = 0;}
            dataPoints[i] = mpBuffer[k];
        }
    }
    
    return(lagrangeInterp(dataPoints, interpSampN, targetX));
    
}
