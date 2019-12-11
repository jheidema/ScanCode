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
        int nBP = 0;
        vector<double> params;
        Double_t pArray[npars];
        Double_t *bArray;
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
    void GetFuncParams(int iF){cout << params.at(nBP+iF*2) << " " << params.at(nBP+1+iF*2)<< endl;};
    void GetBkgdParams(){for (int i=0; i<nBP; i++) cout << i << " " << params.at(i) << endl;};
    //void RemoveFunction();
    double operator() (double *x, double *p){
        if(kFit) SetParams(p);
        Double_t res = 0.;
        //Double_t bArray[nP];
        //for(int in=0;in<nP;in++) bArray[in] = params.at(in);
            res = FullBkgd(x,bArray);

        for (int i=0; i<nFuncs; i++){
            for (int in=0; in<npars; in++) pArray[in] = params.at(nBP+i*npars+in);
            res += calcResponse(x,pArray);
        } 
        return res;   
    };
};

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

