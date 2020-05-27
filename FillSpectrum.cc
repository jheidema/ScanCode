
#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraph.h"
#include "TObject.h"

#include "fullFuncClass.hpp"
#include "optionHandler.hpp"
#include "FullSpecFunc.hpp"
#include "HFReader.hpp"
#include "ConfigReader.hpp"

#include "VandleEff.hpp"

using namespace std;

string errstring = "[ERROR] FillSpectrum::main >>> ";

int main(int argc, char **argv){
     
  optionHandler handler;
  handler.add(optionExt("HFinput",required_argument, NULL,'i',"<HFfilename>","Specify an input file from HF calculations (can't be used with -M)."));
  handler.add(optionExt("HFmulti",required_argument, NULL,'M',"<HFfilename>","Specify text file with paths to HF calculations (can't be used with -i)."));
  handler.add(optionExt("output", required_argument, NULL,'o', "<filename>", "Specify the name of the output file."));
  handler.add(optionExt("Config",required_argument,NULL,'c',"<ConfigFile>","Specify Config file for setting intensities and ROOT stogram file"));
  handler.add(optionExt("verbose",optional_argument,NULL,'v',"","Print information for GS feeding and Ex Branching Ratios"));
  //handler.add(optionExt("Histinput",required_argument, NULL,'H',"<HistFilename>","Specify Root File with TOF histogram."));
  //handler.add(optionExt("LevelInfo",required_argument,NULL,'l',"<LevelFile>","Specify file with level structure info"));
  //handler.add(optionExt());

  if (argc<2) {cout << errstring << "Incorrect number of arguments.\n";
   handler.help(argv[0]); 
   return 1;
  }

  if(!handler.setup(argc, argv))
	  return 1;

  if(handler.getOption(0)->active&&handler.getOption(1)->active){
    cout << errstring << "Options [-i] and [-M] must be used exclusively." << endl;
    return 1;
  }
  
  std::string inputFilename;
  bool HFMulti;
	if(handler.getOption(0)->active){
	  inputFilename = handler.getOption(0)->argument;
    HFMulti = false;
  } 
	else if(handler.getOption(1)->active){
    inputFilename = handler.getOption(1)->argument;
    HFMulti = true;
  }
  else{
    cout << errstring << "Input filename not specified!\n" ;
	  return 1;
	}

  std::string outputFilename;
	if(handler.getOption(2)->active) // Set input filename
	  outputFilename = handler.getOption(2)->argument;        
  else {
    cout << "No output name provided. Using default \"fund_output.root\"\n";
    outputFilename = "func_output.root";
  }
  
  std::string ConfigFilename;
	if(handler.getOption(3)->active) // Set Config filename
	  ConfigFilename = handler.getOption(3)->argument;

  cout << "---Input Info---\n";
  cout << "---Reading from " << inputFilename << endl;
  cout << "---Outputing to " << outputFilename << endl; 
  cout << "---Opening Config file " << ConfigFilename << endl;
  
  std::string RootFilename;
  std::string LevelFilename;
  
  bool kVerbose = false;
  if(handler.getOption(4)->active) kVerbose = true;


  ConfigReader cr;
  if(!(cr.SetFileName(ConfigFilename.c_str()))){
     cout << errstring << "Config file does not exist.\n"; 
     return 1;
  }
  
  cr.Import();
  RootFilename = cr.GetHistogramName();
  LevelFilename = cr.GetLevelName();
  bool gsFlag = cr.GetGSFlag();
  bool gsFit = cr.GetGSFitFlag();
  if(gsFlag && gsFit) {
      cout << errstring << "Flag set for GS branching ratio and fitting. Must be exclusive.\n"; 
      return 1;
  }

  cout << "---Setting neutron peaks from " << LevelFilename << endl;
  cout << "---Applying to histogram from " << RootFilename << "\n\n" << endl;

  
  HFReader hfreader;
  if(HFMulti){
    hfreader.MultiFile(inputFilename.c_str());
  }else{
    hfreader.SetFileName(inputFilename.c_str());
    hfreader.Import();
  }
  vector < vector<double> > HFspectra = hfreader.GetVectors();
  vector <double> HFBins = hfreader.GetBins();
  double nNorm=0.0;
  for (int ir=0;ir<HFspectra.size();ir++) nNorm += HFspectra.at(ir).at(1);

  //Load Histogram from root
  TFile *fIn = new TFile(RootFilename.c_str(),"READ");
  if(!fIn->IsOpen()) {cout << errstring << "ROOT file failed to open"<< endl;
    return 1;
  }

  TList *fKeys = fIn->GetListOfKeys();
  fKeys->ls();
  std::string hname;
  cout << "Please enter key for histogram of interest [default=\'h\']\n";
  getline(cin,hname);
    if(hname.empty()) hname = "h";

  if(!(fKeys->Contains(hname.c_str()))) {
    cout << errstring << "Object " << hname << " does not exist.\nExiting....\n";
    return 1;
  }
  
  TH1D *hIn; 
  string hn1 = "TH1D"; string hn2 = "TH1F";
  const char *hType = (fIn->Get(hname.c_str()))->ClassName();
  cout << "Histogram Type: " << hType << endl;
  
  if(hn2.compare(hType)==0) {
    cout << "Converting Histogram" << endl;
    hIn = (TH1D*)fIn->Get(hname.c_str());
  }
  else if(hn1.compare(hType)==0) hIn = (TH1D*)fIn->Get(hname.c_str());
  else {cout << errstring << "Improper Histogram Type. Only TH1D or TH1F\nExiting....\n"; return 1;}; 

  int db = 50;
  int histN = hIn->Integral(hIn->FindBin(25),hIn->FindBin(25+db))-hIn->Integral(hIn->FindBin(300),hIn->FindBin(300+db));
  if(histN<100) {
    cout << errstring << "Histogram is invalid for processing [Too Few Entries].\nExiting....\n";
    return 1;
  }  
  
  FullSpecFunc ff(kVerbose);
  ff.SetInfoFile(LevelFilename.c_str());
  ff.SetGSCalc(gsFlag);
    
  ff.GenerateSpecFunc(hIn, false);
   
  ///Making functions from HF calculations
  FullFuncClass sf;
  TGraph *gEff = Efficiencies::VandleEff();
  for (int i=0;i<HFBins.size();i++){
    double amp = HFspectra.at(i).at(1);
    if (amp>0.05){
     double tof=sqrt(1./HFBins.at(i)*0.5*939.6/pow(29.98,2))*100.;
     double cEff = gEff->Eval(HFBins.at(i));
     
     sf.InsertFunction(tof,amp*(double)histN/nNorm*0.8*cEff/100.); /// JNH: 0.8 FACTOR NEEDS TO BE REMOVED BEFORE SETTING PROPER INTENSITIES
    }   
  }
  
  //Final corrections before writing to file
  hIn->GetXaxis()->SetRangeUser(20,200);
  
  hIn->GetYaxis()->UnZoom();
  hIn->SetLineColor(kBlack);
  hIn->SetLineWidth(2);
  hIn->SetTitle(inputFilename.c_str());

  TF1 *fH;
  TF1 *fN;
  if(gsFit){
     fH = ff.FitGSStates(hIn);
     fN = ff.GetFunc();
     fN->SetRange(0,800);
     fN->SetNpx(5000);
  } else fH = ff.GetFunc();

  fH->SetRange(0,800);
  fH->SetNpx(5000);

  TF1 *fS = new TF1("fS",sf,0,800,0);
  fS->SetNpx(5000);
  fS->SetLineColor(kBlue);

  TFile *fOut = new TFile(outputFilename.c_str(),"RECREATE");
  hIn->Write();
  fH->Write();
  if(gsFit) fN->Write();
  fS->Write();

  fOut->Close();
  fIn->Close();
  
  
  return 0;
}
