#include <iostream>
#include <cmath>
#include <vector>

#include "calcResponse.hpp"

using namespace std;

class tofFuncClass {
    public:
        Int_t nFuncs;
        vector<double> params;
        double pArray[3];
        

    tofFuncClass();
    ~tofFuncClass();
    void Clear();
    void InsertFunction(double t0, double amp, double sigE = 0.0);
    void SetParams(double *p);
    void SetParams(int iF, double *p);
    void SetParam(int iF, int iP, double p);
    //void RemoveFunction();
    double operator() (double *x, double *p){
        SetParams(p);
        double res = 0.0;
        double off = p[nFuncs*3];
        for (int i=0; i<nFuncs; i++){
            pArray[0] = p[i*3]; pArray[1] = p[i*3+1]; pArray[2] = p[i*3+2];
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
    params.push_back(sigE);
//    params.push_back(oFF);
}

void tofFuncClass::SetParams(double *p){
    for(int ip=0; ip<nFuncs*3; ip++){
        params.at(ip) = p[ip];
    }
}

void tofFuncClass::SetParams(int iF, double *p){
    for(int ip=0; ip<3; ip++){
        params.at(ip+(iF*3)) = p[ip];
    }
}

void tofFuncClass::SetParam(int iF, int iP, double p){
    params.at(iP+iF*3) = p;
}

void tofFuncClass::Clear(){
    nFuncs = 0;
    params.clear();
    for (int ip=0; ip<4; ip++) pArray[ip] = 0.0;
}

