#include <stdio.h>
#include <string.h>
#include <sstream>
#include <istream>
#include <iterator>
#include "FileReader.h"


#ifdef FileReader_hpp
using namespace std;

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
    
    if(!(SetFileName(filename_))) return false;
    
    bool kGood = OpenFile();
    return kGood; 
}

vector<vector<string>> FileReader::LoadFile(){

    GetLine();
    nL=0;
    while(infile.good()){
        getline(infile,lineString);
        nL++;
        auto found = lineString.find_first_not_of(" \t");
        if( found != string::npos){
            if( lineString[found] != '#'){
                std::stringstream ss(lineString);
                std::istream_iterator<std::string> first(ss);
                std::istream_iterator<std::string> last;
                std::vector<std::string> linevec(first, last);
                //std::copy(vstrings.begin(), vstrings.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
                ftext.push_back(linevec);
                if(kVerbose) cout << "[FILE READER (" << filename << ")]: " << lineString.c_str() << endl;
            }
        }
    }
    if(kVerbose) cout << "Loaded File " << filename << endl;
    return ftext;
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