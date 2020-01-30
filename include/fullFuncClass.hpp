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
    bool SetFitBool(bool rFit){kFit = rFit; return kFit;};
    int GetNFuncs() {return nFuncs;};
    void GetFuncParams(int iF){cout << params.at(nBP+iF*2) << " " << params.at(nBP+1+iF*2)<< endl;};
    void GetBkgdParams(){for (int i=0; i<nBP; i++) cout << i << " " << params.at(i) << endl;};
    //void RemoveFunction();
    double operator() (double *x, double *p){
        if(kFit) SetParams(p);
        double res = 0.;
        //double bArray[nP];
        //for(int in=0;in<nP;in++) bArray[in] = params.at(in);
            res = FullBkgd(x,bArray);

        for (int i=0; i<nFuncs; i++){
            for (int in=0; in<npars; in++) pArray[in] = params.at(nBP+i*npars+in);
            res += calcResponse(x,pArray);
            //res = 0;
        } 
        return res;   
    };
};

#endif