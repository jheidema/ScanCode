#include <stdio.h>
#include <string.h>

#include "FileReader.hpp"


#ifdef FileReader_hpp


FileReader::FileReader(){
};

FileReader::FileReader(const char *filename_){
    OpenFile(filename_);
}

FileReader::~FileReader(){
    if(infile.is_open()) infile.close();
};

std::string FileReader::GetLine(){
    if(infile.good()) getline(infile,lineString);
    return lineString;
}

bool FileReader::OpenFile(){
    if(std::string(filename).empty()) return false;
    
    if(!infile.is_open())
      infile.open(filename,std::ifstream::in);
    return true;
}

bool FileReader::OpenFile(const char* filename_){
    if(infile.is_open()) return false;
    
    SetFileName(filename_);
    OpenFile();
    return true; 
}

bool FileReader::Import(){
    
    infile.open(filename,std::ifstream::in);

    getline(infile,lineString);
    nL=0;

    while(infile.good()){
        nL++;
        cout << lineString.c_str() << endl;
        getline(infile,lineString);
    }

  return true; 
}

#endif