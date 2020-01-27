#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "dummyFuncs.h"
//#include "BkgdFunc.C"
//#include "HypBkgdFunc.C"
//#include "PieceBkgdFunc.C"
//#include "SpectrumFunc.hpp"
#include "fullFuncClass.hpp"

#include "TH1F.h"
#include "TF1.h"

#define FullSpecFunc_hpp

class FullSpecFunc {
    public:
    FullSpecFunc();
    virtual ~FullSpecFunc();

    void GenerateSpecFunc(TH1F * hIn, Bool_t kHyp=false);
    TF1* GetFunc(){return fOut;};
    FullFuncClass GetFuncObj(){return tf;}
    void SetInfoFile(const char *filename_){filename = filename_;};    
    
    
    const int nrp = 2;
    const char *filename;
    TF1 *fB;
    FullFuncClass tf;
    TF1 *fOut;
    double *bArray;
    int nBP;
    int nFuncs;

};