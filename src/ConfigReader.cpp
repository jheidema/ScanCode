#include <stdio.h>
#include <string>
#include <sstream>

#include "ConfigReader.hpp"


#ifdef ConfigReader_hpp


ConfigReader::ConfigReader(){
  SetValidNames();
  stob.insert(std::make_pair("true",true));
  stob.insert(std::make_pair("false",false));
};

ConfigReader::~ConfigReader(){
};

void ConfigReader::SetValidNames(){
  valids.insert(std::make_pair("RootHist","hname"));
  valids.insert(std::make_pair("RootFile","hname"));

  valids.insert(std::make_pair("LevelName","lname"));
  valids.insert(std::make_pair("LevelInfo","lname"));

  valids.insert(std::make_pair("OutputFile","output"));
  valids.insert(std::make_pair("OutputName","output"));

  valids.insert(std::make_pair("InputFile","input"));
  valids.insert(std::make_pair("InputName","input"));

  valids.insert(std::make_pair("GSRatio","brname"));
  valids.insert(std::make_pair("BRFile","brname"));
  
  valids.insert(std::make_pair("GSFlag","gsflag"));
  valids.insert(std::make_pair("gsFlag","gsflag"));
  valids.insert(std::make_pair("gsflag","gsflag"));

  valids.insert(std::make_pair("GSFit","gsfit"));
  valids.insert(std::make_pair("gsFit","gsfit"));
  valids.insert(std::make_pair("gsfit","gsfit"));
  valids.insert(std::make_pair("GSfit","gsfit"));

  valids.insert(std::make_pair("GSScale","gscalc"));
  valids.insert(std::make_pair("gsScale","gscalc"));

  return;
}

bool ConfigReader::IsValid(std::string name){
  if(valids.count(name)==0) return false;
  else return true;
}

void ConfigReader::PrintLine(std::string line){
  cout << line << endl;
}

bool ConfigReader::Import(){
  bool kRead = true;
  //cout << "ConfigReader::Import: Reading Config File\n";
  std::ifstream infile;
  infile.open(filename,std::ifstream::in);
  getline(infile,lineString);
  nL=0;
  std::string first, second;
  while(infile.good()){
      nL++;
      //cout << lineString.c_str() << endl;
      auto found = lineString.find_first_not_of(" \t");
      if( found != string::npos){
      if( lineString[found] != '#'){

        std::stringstream ss(lineString);
        ss >> first >> second;
        if(IsValid(first)) namemap.insert(std::make_pair(valids[first],second));  
        
        else {
          cout << "Config Error: File name not allowed. Rename key " << first << "\n";
          kRead = false;
          break;
          }
        }
      }
      getline(infile,lineString);
  }
return kRead; 
}

#endif