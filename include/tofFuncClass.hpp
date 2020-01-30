#ifndef TofFuncClass_hpp
#define TofFuncClass_hpp

#include <iostream>
#include <cmath>
#include <vector>

//#include "calcResponse.hpp"

using namespace std;

class tofFuncClass {
    public:
        int nFuncs;
        static const int npars = 2;
        vector<double> params;
        double pArray[npars];
        

    tofFuncClass();
    ~tofFuncClass();
    void Clear();
    void InsertFunction(double t0, double amp=1000, double sigE = 0.0);
    void SetParams(double *p);
    void SetParams(int iF, double *p);
    void SetParam(int iF, int iP, double p);
    //void RemoveFunction();
    double operator() (double *x, double *p){
        //SetParams(p);
        double res = 0.0;
        int nF = p[0];
        double off = p[nF*npars+1];
        for (int i=0; i<nF; i++){
            for (int in=1; in<=npars; in++){pArray[in-1] = p[i*npars+in];}// pArray[1] = p[i*npars+2];// pArray[2] = p[i*3+3];
            //res += calcResponse(x,pArray);
            res = 0;
        } 
        return res + off;   
    };
};

#endif