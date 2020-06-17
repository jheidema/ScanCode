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
    return;
}

void FitFuncClass::AddGaus(double mean, double sigma, double amp){
    g_means.push_back(mean); 
    g_sigs.push_back(sigma); 
    g_amps.push_back(amp); 
    nGaus++;
    
    return;
}

void FitFuncClass::AddLandau(double mpv, double sigma, double amp){
    l_mpvs.push_back(mpv);
    l_sigmas.push_back(sigma);
    l_amps.push_back(amp);
    nLand++;
    
    return;
}

void FitFuncClass::AddNuFunc(double tof, double amp){
    n_tofs.push_back(tof);
    n_amps.push_back(amp);
    nNu++;
    
    return;
}
#endif