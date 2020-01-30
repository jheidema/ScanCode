
#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TH1F.h"

//#include "fullFuncClass.hpp"
#include "optionHandler.hpp"
#include "FullSpecFunc.hpp"
#include "HFReader.hpp"
#include "ConfigReader.hpp"

using namespace std;

int main(int argc, char *argv[]){
    optionHandler handler;
        handler.add(optionExt("HFinput",required_argument, NULL,'i',"<HFfilename>","Specify an input file from HF calculations."));
        handler.add(optionExt("output", required_argument, NULL, 'o', "<filename>", "Specify the name of the output file."));
        //handler.add(optionExt("Histinput",required_argument, NULL,'H',"<HistFilename>","Specify Root File with TOF histogram."));
        handler.add(optionExt("Config",required_argument,NULL,'c',"<ConfigFile>","Specify Config file for setting intensities"));
        //handler.add(optionExt("LevelInfo",required_argument,NULL,'l',"<LevelFile>","Specify file with level structure info"));

        //handler.add(optionExt());

    if(!handler.setup(argc, argv))
		return 1;

    std::string inputFilename;
	  if(handler.getOption(0)->active) // Set input filename
		inputFilename = handler.getOption(0)->argument;

    std::string outputFilename;
	  if(handler.getOption(1)->active) // Set input filename
		outputFilename = handler.getOption(1)->argument;        
    else outputFilename = "func_output.root";


    //Reads in Histogram and Level info filenames
    std::string ConfigFilename;
	  if(handler.getOption(3)->active) // Set Config filename
		ConfigFilename = handler.getOption(3)->argument;

    std::string RootFilename;
    std::string LevelFilename;
    ConfigReader cr;
    if(!(cr.SetFileName(ConfigFilename.c_str()))) ;
    RootFilename = cr.GetHistogramName();
    LevelFilename = cr.GetLevelName();


    //Load Histogram from root
    TFile *fIn = new TFile(RootFilename.c_str(),"READ");
    //TH1F *hIn = (TH1F*)fIn->Get;


    HFReader hfreader;
    hfreader.SetFileName(inputFilename.c_str());
    hfreader.Import();
    vector < vector<double> > HFspectra = hfreader.GetVectors();
    vector <double> HFBins = hfreader.GetBins();

    FullSpecFunc ff;
    ff.SetInfoFile(LevelFilename.c_str());
    //ff.GenerateSpecFunc(hIn, false);

    //get spectrum function and bkgd function object and
    //add feeding from 
    cout << "Good So Far. Keep Working\n" ;
    fIn->Close();
    return 0;
}