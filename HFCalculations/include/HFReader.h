#ifndef HFReader_hpp
#define HFReader_hpp

#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <string>

//#define HFReader_hpp

#include "FileReader.h"

using namespace std;


class HFReader : public FileReader {
    public:
        HFReader(): FileReader(){};
        ~HFReader();
        bool Import() override;
        void LoadVectors(std::string lineString);
        void PrintInfo();
        
        vector < vector <double> > GetVectors(){return pHists;};
        vector < double >  GetBins();
        vector <double> binLeft;
        vector <double> binRight;
        vector < vector <double> > pHists;

};

#endif
