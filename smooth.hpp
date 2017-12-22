
#ifndef smooth_h
#define smooth_h


class CParamSmooth
{
public:
    CParamSmooth() { a = 0.99; b = 1. - a; z = 0.; };
    ~CParamSmooth() {};
    inline double Process(double in)
    {
        z = (in * b) + (z * a);
        
        z = (z > 0.01) ? z : 0.0;
        return z;
    }
private:
    double a, b, z;
};


#endif /* smooth_h */
