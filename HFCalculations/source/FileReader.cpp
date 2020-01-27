#include <stdio.h>
#include <string.h>

#include "FileReader.hpp"


#ifdef FileReader_hpp


FileReader::FileReader(){
};

FileReader::~FileReader(){
};


bool FileReader::Import(){
    std::ifstream infile;
    infile.open(filename,std::ifstream::in);

    getline(infile,lineString);
    nL=0;

    while(infile.good()){
        nL++;
        cout << lineString.c_str() << endl;
        getline(infile,lineString);
    }

/*     char c = infile.get();

    while (infile.good()) {
    std::cout << c;
    c = infile.get();
    }

  infile.close(); */

  return true; 
}

#endif