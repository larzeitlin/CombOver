//
//  ModMatrix.cpp
//  CombOver
//
//  Created by Luke Zeitlin on 03/08/2017.
//
//

#include "ModMatrix.hpp"
#include <vector>

int ModMatrix::addSource(double* source)
{
    sources.push_back(source);
    return((int)sources.size() - 1);
}

int ModMatrix::addDestination()
{
    double destination = 0.;
    destinations.push_back(destination);
    return((int)destinations.size() - 1);
}


void ModMatrix::buildMatrix()
{
    for(int d = 0; d < destinations.size(); ++d)
    {
        std::vector<double> destinationRow;
        for(int s = 0; s < sources.size(); ++s)
        {
            destinationRow.push_back(0);
        }
        ModMatrix2D.push_back(destinationRow);
    }
}

void ModMatrix::setValInMatrix(double val, int x, int y)
{
    ModMatrix2D[x][y] = val;
}

void ModMatrix::calcMatrix()
{
    for(int d = 0; d < destinations.size(); ++d)
    {
        destinations[d] = 0;
        for(int i = 0; i < sources.size(); ++i)
        {
            destinations[d] += ModMatrix2D[d][i] * *sources[i];
        }
    }
}

double * ModMatrix::getDestinationValAddress(int dIndex)
{
    return (&destinations[dIndex]);
}

double ModMatrix::getValInMatrix( int x, int y)
{
    return(ModMatrix2D[x][y]);
}
