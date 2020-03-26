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
        double pArray[npars];
        double *bArray;
        bool kFit = false;    

    FullFuncClass(){};
    FullFuncClass(int nP, double *initP);
    ~FullFuncClass();
    void Clear();
    void InsertFunction(double t0, double amp=1000);
    void SetParams(double *p);
    void SetParams(int iF, double *p);
    void SetParam(int iF, int iP, double p);
    void GetParam(int iP) {return params.at(iP);};
    bool SetFitBool(bool rFit){kFit = rFit; return kFit;};
    int  GetNFuncs() {return nFuncs;};
    int  GetNBP() {return nBP;};
    void PrintFuncParams(int iF){cout << params.at(nBP+iF*npars) << " " << params.at(nBP+1+iF*npars)<< endl;};
    void PrintFuncParams(){for(int iF=0;iF<nFuncs;iF++) cout << params.at(nBP+iF*npars) << " " << params.at(nBP+1+iF*npars)<< endl;};
    void PrintBkgdParams(){for(int i=0;i<nBP;i++)       cout << i << " " << params.at(i) << endl;};
    
    vector<double> GetParams(){return params;};

    //void RemoveFunction();
    double operator() (double *x, double *p){
        if(kFit) SetParams(p);
        double res = 0.;
        
        if (nBP>0) res = FullBkgd(x,bArray);

        for (int i=0; i<nFuncs; i++){
            for (int in=0; in<npars; in++) pArray[in] = params.at(nBP+i*npars+in);
            res += calcResponse(x,pArray);
            //res = 0;
        } 
        return res;   
    };
};

#endif