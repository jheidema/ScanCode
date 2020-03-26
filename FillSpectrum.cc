
#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"

#include "fullFuncClass.hpp"
#include "optionHandler.hpp"
#include "FullSpecFunc.hpp"
#include "HFReader.hpp"
#include "ConfigReader.hpp"

#include "VandleEff.h"

using namespace std;

int main(int argc, char **argv){
     
  optionHandler handler;
  handler.add(optionExt("HFinput",required_argument, NULL,'i',"<HFfilename>","Specify an input file from HF calculations."));
  handler.add(optionExt("output", required_argument, NULL,'o', "<filename>", "Specify the name of the output file."));
  handler.add(optionExt("Config",required_argument,NULL,'c',"<ConfigFile>","Specify Config file for setting intensities and ROOT stogram file"));
  //handler.add(optionExt("Histinput",required_argument, NULL,'H',"<HistFilename>","Specify Root File with TOF histogram."));
  //handler.add(optionExt("LevelInfo",required_argument,NULL,'l',"<LevelFile>","Specify file with level structure info"));
  //handler.add(optionExt());
  if (argc<2) {cout << "Incorrect number of arguments.\n";
   handler.help(argv[0]); 
   return 1;
  }

  if(!handler.setup(argc, argv))
	  return 1;

  std::string inputFilename;
	if(handler.getOption(0)->active) // Set input filename
	  inputFilename = handler.getOption(0)->argument;
   //if(inputFilename.empty()){
	else{
    cout << "Input filename not specified!\n" ;
	  return 1;
	}

  std::string outputFilename;
	if(handler.getOption(1)->active) // Set input filename
	  outputFilename = handler.getOption(1)->argument;        
  else {
    cout << "No output name provided. Using default \"fund_output.root\"\n";
    outputFilename = "func_output.root";
  }
  
  std::string ConfigFilename;
	if(handler.getOption(2)->active) // Set Config filename
	  ConfigFilename = handler.getOption(2)->argument;

  cout << "---Input Info---\n";
  cout << "---Reading from " << inputFilename << endl;
  cout << "---Outputing to " << outputFilename << endl; 
  cout << "---Opening Config file " << ConfigFilename << endl;
  
  std::string RootFilename;
  std::string LevelFilename;
  ConfigReader cr;
  if(!(cr.SetFileName(ConfigFilename.c_str()))){
     cout << "Config file does not exist.\n"; return 1;
  }
  
  cr.Import();
  RootFilename = cr.GetHistogramName();
  LevelFilename = cr.GetLevelName();
  cout << "---Setting neutron peaks from " << LevelFilename << endl;
  cout << "---Applying to histogram from " << RootFilename << "\n\n" << endl;
  
  
  HFReader hfreader;
  hfreader.SetFileName(inputFilename.c_str());
  hfreader.Import();
  vector < vector<double> > HFspectra = hfreader.GetVectors();
  vector <double> HFBins = hfreader.GetBins();

  FullSpecFunc ff;
  ff.SetInfoFile(LevelFilename.c_str());
  
  //Load Histogram from root
  TFile *fIn = new TFile(RootFilename.c_str(),"READ");
  TList *fKeys = fIn->GetListOfKeys();
  fKeys->Print();
  std::string hname;
  cout << "Please enter key for histogram of interest [default=\'h1\']\n";
  getline(cin,hname);
    if(hname.empty()) hname = "h1";

  if(fKeys->FindObject(hname.c_str())==0) {
    cout << "[ERROR] FillSpectrum::main >>> Object " << hname << " does not exist.\nExiting....\n";
    return 1;
  }
  
  TH1F *hIn = (TH1F*)fIn->Get(hname.c_str());
  if(hIn->GetEntries()<100) {
    cout << "[ERROR] FillSpectrum::main >>> Histogram is invalid for processing.\nExiting....\n";
    return 1;
  }  
  ff.GenerateSpecFunc(hIn, false);
  FullFuncClass tf = ff.GetFuncObj();
  //tf.PrintBkgdParams();


  ///Making functions from HF calculations
  FullFuncClass sf;
  TGraph *gEff = VandleEff();
  for (int i=0;i<HFBins.size();i++){
    double amp = HFspectra.at(i).at(1);
    if (amp>0.05){
     double tof=sqrt(1./HFBins.at(i)*0.5*939.6/pow(29.98,2))*100.;
     double cEff = gEff->Eval(HFBins.at(i));
     cout << "Adding function at " << tof << endl;
     sf.InsertFunction(tof,amp*1.5*200.*cEff/100.);
    }   
  }
  sf.PrintFuncParams();


  //Final corrections before writing to file
  hIn->GetXaxis()->SetRangeUser(30,150);
  hIn->GetYaxis()->SetRangeUser(0,hIn->GetBinCenter(hIn->GetMaximumBin()));
  hIn->SetLineColor(kBlack);
  hIn->SetLineWidth(2);
  hIn->SetTitle(inputFilename.c_str());

  TF1 *fH = ff.GetFunc();
  fH->SetRange(0,800);
  fH->SetNpx(5000);

  TF1 *fS = new TF1("fS",sf,0,800,0);
  fS->SetNpx(5000);
  fS->SetLineColor(kBlue);

  TFile *fOut = new TFile(outputFilename.c_str(),"RECREATE");
  hIn->Write();
  fH->Write();
  fS->Write();

  fOut->Close();
  fIn->Close();
  //get spectrum function and bkgd function object and
  //add feeding from 
  //cout << "Good So Far. Keep Working\n" ;
  
  return 0;
}
