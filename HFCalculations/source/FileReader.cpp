#include <stdio.h>
#include <string.h>
#include <sstream>
#include <istream>
#include <iterator>
#include "FileReader.hpp"


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
    if(std::string(filename).empty()){
        if(kVerbose) cout << "[FileReader::OpenFile] No file declared" << endl;
        return false;
    }
    if(!infile.is_open()){
      if(kVerbose) cout << "[FileReader::OpenFile] Opening file " << filename << endl;
      infile.open(filename,std::ifstream::in);
    return true;
    }
    else {
      if (kVerbose) cout << "[FileReader::OpenFile] File " << filename << " already open." << endl;
    return false;
    }
    return false;
}

bool FileReader::OpenFile(const char* filename_){
    if(infile.is_open()) return false;
    
    if(!(SetFileName(filename_))) return false;
    
    bool kGood = OpenFile();
    return kGood; 
}

bool FileReader::CloseFile(){
    if(!infile.is_open()){
        cout << "[FileReader::CloseFile] No File Open." << endl;
        return false;
    }
    if(kVerbose) cout <<"[FileReader::CloseFile] Closing File " << filename << endl;
    infile.close();
    return true;
}

vector<string> FileReader::GetRow(){
    vector<string> non;
    //if(kVerbose) cout << "[FileReader::GetR] Loading Summary File: " << filename << endl;
    nL=0;
    if(!infile.good()) {
        cout << "[FileReader::LoadFile] File isn't good." << endl;
        return non;
    }
    getline(infile,lineString);
    
    auto found = lineString.find_first_not_of(" \t");
            std::stringstream ss(lineString);
            std::istream_iterator<std::string> first(ss);
            std::istream_iterator<std::string> last;
            std::vector<std::string> linevec(first, last);
            
    if( found != string::npos && lineString[found] != '#')
        return linevec;

    else return non;
}
vector<vector<string>> FileReader::LoadFile(){
    ftext.clear();
    if(kVerbose) cout << "[FileReader::LoadFile] Loading Summary File: " << filename << endl;
    //GetLine();
    //getline(infile,lineString);
    nL=0;
    if(!infile.good()) {
        cout << "[FileReader::LoadFile] File isn't good." << endl;
        return ftext;
    }
    while(infile.good()){
        //if(nL!=0)
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
                if(linevec.size()>0){ 
                    if(kVerbose) cout << "[FILE READER (" << filename << ")]: " << lineString.c_str() << endl;
                    ftext.push_back(linevec);}
                
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