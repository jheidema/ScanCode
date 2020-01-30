#include "tofFuncClass.hpp"

#ifdef TofFuncClass_hpp

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

#endif