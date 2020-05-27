#ifndef FullFuncClass_hpp
#define FullFuncClass_hpp

#include <iostream>
#include <cmath>
#include <vector>

#include "calcResponse.hpp"
#include "dummyFuncs.hpp"

using namespace std;

class FullFuncClass {
    public:
        int nFuncs=0;
        static const int npars = 2;
        int nBP = 0;
        vector<double> params;
        //vector<double> BGparams;
        double pArray[npars];
        double *bArray;
        bool kFit = false;    

    FullFuncClass();
    FullFuncClass(int nP, double *initP);
    ~FullFuncClass();
    void Clear();
    void InsertFunction(double t0, double amp=1000);
    void SetParams(double *p);
    void SetParams(int iF, double *p);
    void SetParam(int iF, int iP, double p);
    void SetBGParams(int nP, double *bkP);
    void SetFitBool(bool kIn){kFit = kIn;};
    void SetNFuncs(int nfIn){nFuncs=nfIn;};
    
    double GetParam(int iP) {return params.at(iP);};
    bool   GetFitBool(){return kFit;};
    int  GetNFuncs() {return nFuncs;};
    int  GetNBP() {return nBP;};
    
    void PrintFuncParam(int iF){cout << params.at(iF*npars) << " " << params.at(1+iF*npars)<< endl;};
    void PrintFuncParams(){for(int iF=0;iF<nFuncs;iF++) cout << params.at(iF*npars) << " " << params.at(1+iF*npars)<< endl;};
    void PrintBkgdParams(){for(int i=0;i<nBP;i++) cout << i << " " << bArray[i] << endl;};
    
    vector<double> GetParams(){return params;};

    //void RemoveFunction();
    double operator() (double *x, double *p){
        double res = 0.;
        if (nBP>0) res = FullBkgd(x,bArray);
        //if(kFit) for(int i=0; i<nFuncs*npars; i++) params.at(i) = p[i];
        
        if(kFit){// SetParams(p);
            for (int i=0; i<nFuncs; i++){
                for (int in=0; in<npars; in++) pArray[in] = p[i*npars+in];
                res += calcResponse(x,pArray);
            } 
        } else {
            for (int i=0; i<nFuncs; i++){
                for (int in=0; in<npars; in++) pArray[in] = params.at(i*npars+in);
                res += calcResponse(x,pArray);
            }
        }
        
        return res;   
    };
};

#endif