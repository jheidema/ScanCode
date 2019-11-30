#include <iostream>
#include <cmath>
#include <vector>

#ifndef calcResponse_hpp
#include "calcResponse.hpp"
#endif
#ifndef dummyFunc_h
#include "dummyFuncs.h"
#endif


#define FullFuncClass_hpp

using namespace std;

class FullFuncClass {
    public:
        Int_t nFuncs=0;
        static const int npars = 2;
        vector<double> params;
        Double_t pArray[npars];
        Double_t bArray[11];
        Bool_t kFit = false;    

    //FullFuncClass();
    FullFuncClass(int nP, double *initP);
    ~FullFuncClass();
    void Clear();
    void InsertFunction(double t0, double amp=1000);
    void SetParams(double *p);
    void SetParams(int iF, double *p);
    void SetParam(int iF, int iP, double p);
    Bool_t SetFitBool(Bool_t rFit){kFit = rFit; return kFit;};
    Int_t GetNFuncs() {return nFuncs;};
    void GetFuncParams(int iF){cout << params.at(11+iF*2) << " " << params.at(11+1+iF*2)<< endl;};
    
    //void RemoveFunction();
    double operator() (double *x, double *p){
        if(kFit) SetParams(p);
        Double_t res = 0.;
        for(int in=0;in<11;in++) bArray[in] = params.at(in);
            res = FullBkgd(x,bArray);

        for (int i=0; i<nFuncs; i++){
            for (int in=0; in<npars; in++) pArray[in] = params.at(11+i*npars+in);
            res += calcResponse(x,pArray);
        } 
        return res;   
    };
};

FullFuncClass::FullFuncClass(int nP, double *initP){
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
        params.at(ip+11) = p[ip];
    }
}

void FullFuncClass::SetParams(int iF, double *p){
    for(int ip=0; ip<npars; ip++){
        params.at(ip+(iF*npars)+11) = p[ip];
    }
}

void FullFuncClass::SetParam(int iF, int iP, double p){
    params.at(iP+iF*npars+11) = p;
}

void FullFuncClass::Clear(){
    nFuncs = 0;
    params.clear();
    for (int ip=0; ip<npars; ip++) pArray[ip] = 0.0;
}

