#include <iostream>
#include <cmath>
#include <vector>

#ifndef calcResponse_hpp
#include "calcResponse.hpp"
#endif

using namespace std;

class tofFuncClass {
    public:
        Int_t nFuncs;
        static const int npars = 2;
        vector<double> params;
        Double_t pArray[npars];
        

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
        Int_t nF = p[0];
        double off = p[nF*npars+1];
        for (int i=0; i<nF; i++){
            for (int in=1; in<=npars; in++){pArray[in-1] = p[i*npars+in];}// pArray[1] = p[i*npars+2];// pArray[2] = p[i*3+3];
            res += calcResponse(x,pArray);
        } 
        return res + off;   
    };
};

tofFuncClass::tofFuncClass(void){
}

tofFuncClass::~tofFuncClass(){
    Clear();
}

void tofFuncClass::InsertFunction(double t0, double amp, double sigE){
    nFuncs++;
    params.push_back(t0);
    params.push_back(amp);
//    params.push_back(sigE);
//    params.push_back(oFF);
}

void tofFuncClass::SetParams(double *p){
    for(int ip=0; ip<nFuncs*npars; ip++){
        params.at(ip) = p[ip+1];
    }
}

void tofFuncClass::SetParams(int iF, double *p){
    for(int ip=0; ip<npars; ip++){
        params.at(ip+(iF*npars)) = p[ip+1];
    }
}

void tofFuncClass::SetParam(int iF, int iP, double p){
    params.at(iP+iF*npars) = p;
}

void tofFuncClass::Clear(){
    nFuncs = 0;
    params.clear();
    for (int ip=0; ip<npars; ip++) pArray[ip] = 0.0;
}

