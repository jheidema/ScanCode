#include <sstream>
#include <string>
#include <vector>
#include <limits>

#include "../include/HFReader.h"
#include "FileReader.C"

#ifdef HFReader_hpp

HFReader::~HFReader(){
};

void HFReader::LoadVectors(std::string lineString){
  vector <double> colVals;
  std::stringstream ss(lineString); char del = ' ';
  std::string colString;
  while(getline(ss,colString,del)){
    //cout << colString.c_str() << " ";
    if(!colString.empty()){ 
      double val = std::stod(colString);
      colVals.push_back(val);
    }
  }
  //cout << "\n";
  binLeft.push_back(colVals.at(0));
  binRight.push_back(colVals.at(1));
  vector <double> pVals(colVals.begin()+2, colVals.end());
  pHists.push_back(pVals);
  return;
}

void HFReader::PrintInfo(){
  int nR = pHists.size();
  int nC = pHists.at(0).size();

  for (int iR=0; iR < nR; iR++){
    nC = pHists.at(0).size();
    cout << (binLeft.at(iR)+binRight.at(iR))/2.0 << " ";
    for (int iC=0; iC < nC; iC++){
      cout << pHists.at(iR).at(iC) << " ";
    }
    cout << "\n";
  }
  return;
}

vector<double> HFReader::GetBins(){
  vector<double> centerVec;
  if(binLeft.size() == binRight.size()){
    for(int i=0; i<binLeft.size(); i++){
      centerVec.push_back(binLeft.at(i));
      //centerVec.push_back(0.5*(binLeft.at(i)+binRight.at(i)));
    }
  }
  else cout << "HFReader::GetBins >> Bin Vectors are different sizes. Returning Empty Vector.\n";
  
  return centerVec;
}

bool HFReader::Import(){
    std::ifstream infile;
    infile.open(filename,std::ifstream::in);

    getline(infile,lineString);
    nL=0;

    while(infile.good()){
      nL++;
      if (nL > 45){
      auto found = lineString.find_first_not_of(" \t");
      if( found != string::npos){
       if( lineString[found] != '#'){
      //    cout << lineString.c_str() << endl;
        //cout << nL << " ";
        LoadVectors(lineString);  
       }
      }
      if (lineString.length()==0) break;
      }
      getline(infile,lineString);
    }
    infile.close();
  return true; 
}

#endif
