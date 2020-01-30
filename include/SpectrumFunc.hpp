#ifndef SpectrumFunc_hpp
#define SpectrumFunc_hpp

#include <iostream>
#include <fstream>

#include "TF1.h"

void LoadInput(const char *fileName, bool kPrint=false);
TF1* SpectrumFunc(int nPeaks, double *x, double *y, double offset=0.0, double gamma=0.0);
TF1* SpectrumFunc(const char *filename, double offset=0.0, bool kFit=false);

#endif