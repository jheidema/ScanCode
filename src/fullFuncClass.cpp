#include "fullFuncClass.hpp"

#ifdef FullFuncClass_hpp

FullFuncClass::FullFuncClass(){
    kFit=false;
}

FullFuncClass::FullFuncClass(int nP, double *initP){
    SetBGParams(nP,initP);
    kFit=false;
}

FullFuncClass::~FullFuncClass(){
    Clear();
}

void FullFuncClass::InsertFunction(double t0, double amp){
    nFuncs++;
    params.push_back(t0);
    params.push_back(amp);
}

void FullFuncClass::SetBGParams(int nP, double *initP){
    nBP = nP;
    bArray = initP;
}

void FullFuncClass::SetParams(double *p){
    for(int ip=0; ip<nFuncs*npars; ip++){
        params.at(ip) = p[ip];
    }
}

void FullFuncClass::SetParams(int iF, double *p){
    for(int ip=0; ip<npars; ip++){
        params.at(ip+(iF*npars)) = p[ip];
    }
}

void FullFuncClass::SetParam(int iF, int iP, double p){
    params.at(iP+iF*npars) = p;
}

void FullFuncClass::Clear(){
    nFuncs = 0;
    params.clear();
    for (int ip=0; ip<npars; ip++) pArray[ip] = 0.0;
}

#endif