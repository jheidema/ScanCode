#ifndef FullSpecFunc_hpp
#define FullSpecFunc_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

//#include "dummyFuncs.hpp"
//#include "BkgdFunc.C"
//#include "HypBkgdFunc.C"
//#include "PieceBkgdFunc.C"
//#include "SpectrumFunc.hpp"
#include "fullFuncClass.hpp"
#include "FileReader.hpp"

#include "TH1F.h"
#include "TF1.h"


class FullSpecFunc {
    public:
    FullSpecFunc();
    ~FullSpecFunc();

    void SetInfoFile(const char *filename_){filename = filename_;}; 
    bool OpenFile();
    void GenerateSpecFunc();
    void GenerateSpecFunc(TH1F * hIn, bool kHyp=false);
    TF1* GetFunc(){return fOut;};
    FullFuncClass GetFuncObj(){return tf;}
       
    FileReader fr;
    const int nrp = 2;
    const char *filename;
    TF1 *fB;
    FullFuncClass tf;
    TF1 *fOut;
    double *bArray;
    int nBP;
    int nFuncs;

};

#endif