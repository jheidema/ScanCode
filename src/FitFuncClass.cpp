#include "FitFuncClass.hpp"

#ifdef FitFuncClass_hpp

FitFuncClass::FitFuncClass(){
}

FitFuncClass::FitFuncClass(int nP, double *initP){
    SetBGParams(nP,initP);
}

FitFuncClass::~FitFuncClass(){}


void FitFuncClass::SetBGParams(int nP, double *initP){
    nBP = nP;
    bArray = initP;
}

#endif