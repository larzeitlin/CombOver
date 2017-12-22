//
//  ModMatrix.hpp
//  CombOver
//
//  Created by Luke Zeitlin on 03/08/2017.
//
//

#ifndef ModMatrix_hpp
#define ModMatrix_hpp

#include <stdio.h>
#include <vector>

class ModMatrix
{
private:
    std::vector<double*> sources;
    std::vector<double> destinations;
    std::vector< std::vector <double> > ModMatrix2D;
    
public:
    
    int addSource(double* source);
    int addDestination();
    void buildMatrix();
    void setValInMatrix(double val, int x, int y);
    double getValInMatrix(int x, int y);
    void calcMatrix();
    double *getDestinationValAddress(int dIndex);
    
};




#endif /* ModMatrix_hpp */


