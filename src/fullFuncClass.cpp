#include "fullFuncClass.hpp"

#ifdef FullFuncClass_hpp

FullFuncClass::FullFuncClass(int nP, double *initP){
    nBP = nP;
    bArray = initP;
    for(int ip=0;ip<nP;ip++){
        params.push_back(initP[ip]);
    }
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

void FullFuncClass::SetParams(double *p){
    for(int ip=0; ip<nFuncs*npars; ip++){
        params.at(ip+nBP) = p[ip];
    }
}

void FullFuncClass::SetParams(int iF, double *p){
    for(int ip=0; ip<npars; ip++){
        params.at(ip+(iF*npars)+nBP) = p[ip];
    }
}

void FullFuncClass::SetParam(int iF, int iP, double p){
    params.at(iP+iF*npars+nBP) = p;
}

void FullFuncClass::Clear(){
    nFuncs = 0;
    params.clear();
    for (int ip=0; ip<npars; ip++) pArray[ip] = 0.0;
}

#endif