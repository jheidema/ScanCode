#ifndef SpectrumFunc_hpp
#define SpectrumFunc_hpp

#include <iostream>
#include <fstream>
#include <vector>

#include "TF1.h"

const double mn = 939.57; //MeV/c^2
const double c = 2.998E8;
const int npars = 2;

void LoadInput(const char *fileName, bool kPrint=false);
TF1* SpectrumFunc(int nPeaks, double *x, double *y, double offset=0.0, double gamma=0.0);
TF1* SpectrumFunc(const char *filename, double offset=0.0, bool kFit=false);

#endif