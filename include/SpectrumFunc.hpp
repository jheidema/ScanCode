#ifndef SpectrumFunc_hpp
#define SpectrumFunc_hpp

#include <iostream>
#include <fstream>
#include <vector>

#include "TF1.h"

const double mn = 939.57; //MeV/c^2
const double c = 29.98; //cm/ns
const int npars = 2;
const double Sn = 3631; //134Sn neutron separation energy in keV

void LoadInput(const char *fileName, double fGS, bool kPrint=false);
double CalcTOF(double ex);
double CalcEn(double tof);
TF1* SpectrumFunc(int nPeaks, double *x, double *y, double offset=0.0, double gamma=0.0);
TF1* SpectrumFunc(const char *filename, double offset=0.0, double fGS=0.0);

#endif