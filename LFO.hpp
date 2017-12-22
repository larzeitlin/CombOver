//
//  LFO.hpp
//  CombOver
//
//  Created by Luke Zeitlin on 30/07/2017.
//
//

#ifndef LFO_hpp
#define LFO_hpp

#include <stdio.h>
#include <vector>



class LFO
{
private:
    int m_LFOBufferLength;
    double *m_LFOBuffer;
    int m_numerator;
    int m_denominator;
    int m_readSampIndex;
public:
    LFO(int setLength = 1024);
    ~LFO();
    void reset();
    void sine(double phase);
    void setRate(int num, int denom);
    double calcPhaseAndRead(int sampleRate, int currentSample);
    double calcPhaseAndRead(double quaterVal);
    double percentPhase();
    void writeFromStepper(double *stepperVals, int numSteps);
    int getBufferLength();
    void writeFromMSE(double * MSEVals);
};


#endif /* LFO_hpp */
