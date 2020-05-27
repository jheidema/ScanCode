#ifndef HFReader_hpp
#define HFReader_hpp

#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <string>

#include "FileReader.hpp"

using namespace std;


class HFReader : public FileReader {
    public:
        HFReader(): FileReader(){};
        ~HFReader();
        bool Import() override;
        void LoadVectors(std::string lineString);
        void PrintInfo();
        void MultiFile(const char* multiName_);
        
        vector< vector < double > > GetVectors(){return pHists;};
        vector< double >  GetBins();
        void CompileVectors();

        vector< double > binLeft;
        vector< double > binRight;
        vector< vector< double > > pHists;
        
        vector< vector< double > > multiHists;
        vector< double > multiBins;

};

#endif