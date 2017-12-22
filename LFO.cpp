//
//  LFO.cpp
//  CombOver
//
//  Created by Luke Zeitlin on 30/07/2017.
//
//

#include "LFO.hpp"
#include <cstring>
#include <cmath>
#include <iostream>


LFO::LFO(int setLength) : m_LFOBufferLength(setLength)
{
    reset();
}

LFO::~LFO()
{
    delete m_LFOBuffer;
}

void LFO::reset()
{
    m_LFOBuffer = new double[m_LFOBufferLength];
    memset((m_LFOBuffer), 0, m_LFOBufferLength*sizeof(double));
    m_numerator = 1;
    m_denominator = 4;
    m_readSampIndex = 0;
}

void LFO::sine(double phaseRad)
{
    for (int i = 0; i < m_LFOBufferLength ; ++i)
    {
        m_LFOBuffer[i] = sin(  (  (2.* M_PI * i)  / (m_LFOBufferLength)  )  + phaseRad );
    }
}

void LFO::setRate(int num, int denom)
{
    m_numerator = num;
    m_denominator = denom;
}

double LFO::calcPhaseAndRead(int numSamps4Bars, int currentSample)
{
    int sampsPerCycle = (int)(((double)numSamps4Bars / 4.) / (double)m_denominator)*(double)m_numerator;
    
    int adjustedCurrentSample = currentSample % sampsPerCycle;
    
    double decimalPhase = ((double)adjustedCurrentSample / (double)sampsPerCycle);
    int samplePos = (int)((double)m_LFOBufferLength * decimalPhase);
    m_readSampIndex = samplePos;
    return (m_LFOBuffer[samplePos]);
}


double LFO::calcPhaseAndRead(double q)
{
    double Q = (4.*m_numerator) / m_denominator;
    int i =  ( (int)(fmod(q, Q) * m_LFOBufferLength) / Q);
    m_readSampIndex = i;
    return(m_LFOBuffer[i]);   
}

int LFO::getBufferLength()
{
    return m_LFOBufferLength;
}

double LFO::percentPhase()
{
    return( (double(m_readSampIndex) / m_LFOBufferLength) * 100. );
}

void LFO::writeFromStepper(double *stepperVals, int numSteps)
{
    double stepLengthInSamps = m_LFOBufferLength / numSteps;
    
    int currentSampInStep = 0;
    int currentStep = 0;
    for(int i = 0; i < m_LFOBufferLength; ++i)
    {
        m_LFOBuffer[i] = stepperVals[currentStep];

        ++currentSampInStep;
        if (currentSampInStep >= stepLengthInSamps){++currentStep; currentSampInStep = 0;}
    }
}


void LFO::writeFromMSE(double* MSEVals)
{
    for (int i = 0; i < m_LFOBufferLength; ++i)
    {
        m_LFOBuffer[i] = MSEVals[i];
                std::cout<<MSEVals[i]<<std::endl;
        
    }
}
