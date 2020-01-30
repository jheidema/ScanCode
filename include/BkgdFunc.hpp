#ifndef BkgdFunc_hpp
#define BkgdFunc_hpp

#include "dummyFuncs.hpp"

#include "TH1F.h"
#include "TF1.h"

TF1* BkgdFunc(TH1F * hIn, bool kHyp=false);    

#endif