#ifndef TofFuncClass_hpp
#define TofFuncClass_hpp

#include <iostream>
#include <cmath>
#include <vector>

#include "../src/calcResponse.cpp"

using namespace std;

class tofFuncClass {
    public:
        int nFuncs=0;
        static const int npars = 2;
        vector<double> params;
        double pArray[npars];
        bool kFit = false;

    tofFuncClass();
    ~tofFuncClass();
    void Clear();
    void InsertFunction(double t0, double amp=1000);
    void RemoveFunction(int iF);
    void RemovePeak(int iF){SortParams(); RemoveFunction(iF); return;};
    void SortParams();
    
    void SetParams(double *p);
    void SetParams(int iF, double *p);
    void SetParam(int iF, int iP, double p);
    void SetFitBool(bool iFit) {kFit=iFit;};
    
    vector<double> CopyVec(vector<double> vIn);

    bool GetFitBool(){return kFit;}
    double GetParam(int iP) {return params.at(iP);};
    vector<double> GetParams(){return params;};
    int  GetNFuncs(){return nFuncs;};
    void PrintFuncParams(){for(int iF=0;iF<nFuncs;iF++) 
                            cout << iF+1 << " " << params.at(iF*npars) << " " << params.at(iF*npars+1)<< endl;
                        };
    
    double operator() (double *x, double *p){
        if(kFit) for(int i=0; i<nFuncs*npars; i++) params.at(i) = p[i];//SetParams(p);
        double res = 0.0;
        double off = p[nFuncs*npars];
        for (int i=0; i<nFuncs; i++){
            for (int in=0; in<npars; in++){pArray[in] = p[i*npars+in];}// pArray[1] = p[i*npars+2];// pArray[2] = p[i*3+3];
            res += calcResponse(x,pArray);
            //res = 0;
        } 
        return res + off;
    };
};

#endif