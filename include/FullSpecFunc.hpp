#ifndef FullSpecFunc_hpp
#define FullSpecFunc_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "fullFuncClass.hpp"
#include "FileReader.hpp"
#include "SpectrumClass.hpp"

#include "TH1F.h"
#include "TF1.h"


class FullSpecFunc {
    public:
    FullSpecFunc();
    FullSpecFunc(bool kVerbose_);
    ~FullSpecFunc();

    void SetInfoFile(const char *filename_){filename = filename_;}; 
    bool OpenFile();
    void GenerateSpecFunc();
    void GenerateSpecFunc(TH1D *hIn, bool kHyp=false);
    void SetGSCalc(bool kIN) {kGS = kIN;};
    
    FullFuncClass GetFuncObj(){return tf;};
    SpectrumFunc GetSpecFunc(){return sf;};
    
    TF1* GetFunc(){return fOut;};
    TF1* FitGSStates(TH1D* hIn);
    
    FileReader fr;
    SpectrumFunc sf;
    FullFuncClass tf;
    
    const int nrp = 2;
    const char *filename;
    TF1 *fB;
    TF1 *fOut;
    double *bArray;
    int nBP;
    int nFuncs;
    bool kGS = false;
    bool kVerbose = false;

};

#endif