#ifndef SpectrumFunc_hpp
#define SpectrumFunc_hpp

#include <iostream>
#include <fstream>
#include <vector>

#include "TF1.h"


class SpectrumFunc{
     public:
          SpectrumFunc();
          ~SpectrumFunc();
     void LoadInput(const char *fileName, bool kGS, bool kPrint=false);
     double CalcTOF(double ex);
     double CalcEn(double tof);
     TF1* CreateFunc(int nPeaks, double *x, double *y, double offset=0.0, double gamma=0.0);
     TF1* CreateFunc(const char *filename, double offset=0.0, bool kGS=false);

     void SetFeedR(std::map<double,double> feedIn) {feedR.clear(); feedR = feedIn;};
     void SetVerbose(bool kIn){kVerbose=kIn;}
     //void SortGS();
     void PrintExBR();
     std::map<double, std::pair<double, double> > GetGNInfo(){return exBR;};
     std::vector< std::pair<double, double> > GetGSInfo(){return gsInfo;};
     //std::vector< std::pair<double, double> > GetSortedGSInfo(){SortGS(); return gsInfo;};

     std::vector< std::pair<double, double> > stInfo;
     std::vector< std::pair<double, double> > gsInfo;
     std::map<double, std::pair<double, double> > exBR;
     std::map<double, double> feedR;
     bool kVerbose=false;
          
};

#endif