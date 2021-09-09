#include <iostream>
#include <fstream>

#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraph.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "FullFuncClass.hpp"
#include "optionHandler.hpp"
#include "FullSpecFunc.hpp"
#include "HFReader.hpp"
#include "ConfigReader.hpp"

#include "dummyFuncs.hpp"
#include "VandleEff.hpp"


using namespace std;

string errstring = "[ERROR] FillSpectrum::main >>> ";
bool kVerbose = false;
bool kPrintEn = false;

const double mn = 939.57; //MeV/c^2
const double c = 29.98; //cm/ns

void PlotIndividualFuncs(TF1 *fIn, int nNFI, int ng){
  gPad->cd();
  int np = fIn->GetNpar();
  const int nf = np/2;
  TF1 *f1[nf];
  int n = nf-nNFI;
  if(kVerbose) cout << "N Added Functions: " << nNFI << "\nN Input Functions: " << n << endl;
  for (int i=0;i<nf;i++){
    f1[i] = new TF1(Form("nf%d",i),calcResponse,0,500,2);
    f1[i]->SetNpx(3000);
    f1[i]->SetParameters(fIn->GetParameter(i*2),fIn->GetParameter(i*2+1));
    f1[i]->SetLineWidth(2);
    if(i>=ng&&i<n){
      f1[i]->SetLineColor(kBlue); 
      if(fIn->GetParError(i*2)!=0) f1[i]->SetLineStyle(9);
    }
    else if (i<ng) f1[i]->SetLineColor(kMagenta);
    
    else if(i>=n) {f1[i]->SetLineColor(kCyan); f1[i]->SetLineStyle(9);}
    
    f1[i]->Draw("same");
    }

  return;
}

int main(int argc, char **argv){

  gROOT->SetBatch(true);

  optionHandler handler;
  handler.add(optionExt("HFinput",required_argument, NULL,'i',"<HFfilename>","Specify an input file from HF calculations (can't be used with -M)."));
  handler.add(optionExt("HFmulti",required_argument, NULL,'M',"<HFfilename>","Specify text file with paths to HF calculations (can't be used with -i)."));
  handler.add(optionExt("output", required_argument, NULL,'o', "<filename>", "Specify the name of the output file."));
  handler.add(optionExt("Config",required_argument,NULL,'c',"<ConfigFile>","Specify Config file for setting intensities and ROOT stogram file"));
  handler.add(optionExt("verbose",optional_argument,NULL,'v',"","Print debug and information for GS feeding and Ex Branching Ratios"));
  handler.add(optionExt("PrintEnergy",optional_argument,NULL,'P',"","Print deconvolved neutron energies and intensities"));
  
  
  if (argc<2) {cout << errstring << "Incorrect number of arguments.\n";
   handler.help(argv[0]); 
   return 1;
  }

  if(!handler.setup(argc, argv))
	  return 1;

  if(handler.getOption(0)->active && handler.getOption(1)->active){
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
  std::string FunctionFilename;
  
  //kVerbose = false;
  if(handler.getOption(4)->active) kVerbose = true;
  //kPrintEn = false;
  if(handler.getOption(5)->active) kPrintEn = true;


  ConfigReader cr;
  if(!(cr.SetFileName(ConfigFilename.c_str()))){
     cout << errstring << "Config file does not exist.\n"; 
     return 1;
  }
  cr.SetVerbose(kVerbose);
  cr.Import();
  
  RootFilename = cr.GetHistogramName();
  LevelFilename = cr.GetLevelName();
  FunctionFilename = cr.GetFuncFileName();

  bool gsFlag = cr.GetGSFlag();
  bool gsFit = cr.GetGSFitFlag();
  bool kTail = cr.GetTailFitFlag();
  bool kDrawDist = cr.GetDrawDistFlag();
  bool kDrawNFuncs = cr.GetDrawFuncsFlag();
  bool kFloatGS = cr.GetGSFloat();
  bool kGamNuFit = cr.GetGNFit();

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
  std::string hname = fKeys->At(0)->GetName();
  cout << "Please enter key for histogram of interest [default=\'"<< hname << "\']\n";
  getline(cin,hname);
  //if(hname.empty()) hname = "h";
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
  
  hIn->GetXaxis()->UnZoom();
  hIn->GetYaxis()->UnZoom();

  FullSpecFunc ff(kVerbose);
  ff.SetInfoFile(LevelFilename.c_str());
  ff.SetGSCalc(gsFlag);
  ff.SetGSFloat(kFloatGS);
  ff.SetTailFit(kTail);
  ff.SetFuncFileName(FunctionFilename.c_str());
//  ff.GenerateSpecFunc(hIn, false);

  TF1 *fN;
  if(gsFit){
    fN = ff.GenerateFitFunc(hIn,kGamNuFit);
    fN->SetRange(0,800);
    fN->SetNpx(5000);
    fN->SetLineColor(kRed);
  } else{
    cout << "No Fitting Performed" << endl;
    ff.GenerateSpecFunc(hIn, false);
  }   

  TF1 *fH = ff.GetFunc();;
  fH->SetRange(0,800);
  fH->SetNpx(5000);
  fH->SetLineColor(kBlue);

  ///Making functions from HF calculations
  FullFuncClass sf;
  TGraph *gEff = Efficiencies::VandleSourceEff(); //Efficiency curve for analysis

  for (int i=0;i<HFBins.size();i++){
    double amp = HFspectra.at(i).at(1);
    if (amp>0.05){
     double tof=sqrt(1./HFBins.at(i)*0.5*939.6/pow(29.98,2))*100.;
     double cEff = gEff->Eval(HFBins.at(i));
     
     sf.InsertFunction(tof,amp*(double)histN/nNorm*0.8*cEff/100.); /// JNH: 0.8 FACTOR NEEDS TO BE REMOVED BEFORE SETTING PROPER INTENSITIES
    }   
  }

  TF1 *fS = new TF1("fS",sf,0,800,0);
  fS->SetNpx(5000);
  fS->SetLineColor(kBlue);

    //Final corrections before writing to file
  gStyle->SetOptStat(0);
  hIn->GetXaxis()->SetRangeUser(20,200);
  hIn->GetXaxis()->SetTitle("TOF (ns)");
  double ymax =  hIn->GetBinContent(hIn->FindBin(40));
  hIn->GetYaxis()->SetRangeUser(-10,ymax*1.15);
  hIn->GetYaxis()->SetTitle(Form("Counts/%.2f ns",hIn->GetBinWidth(1)));
  //hIn->GetYaxis()->SetTitle("Counts/0.75 ns");
  hIn->SetLineColor(kBlack);
  hIn->SetLineWidth(2);
  hIn->SetTitle(inputFilename.c_str());

  
  TFile *fOut = new TFile(outputFilename.c_str(),"RECREATE");
  hIn->Write();
  fH->Write();
  if(gsFit) fN->Write();
  fS->Write();

  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  hIn->Draw("P E");
  if(gsFit) fN->Draw("same");
  else fH->Draw("same");
  
  
  const int ngs = ff.GetNgauss();
  TF1  *fgs[ngs];
  if(ngs>0&&kDrawDist){
    for (int igs=0; igs<ngs; igs++){
      fgs[igs] = new TF1(Form("fgs%d",igs),gauss,0,500,3);
      fgs[igs]->SetParameters(ff.GetGaussPars(igs));
      fgs[igs]->SetNpx(2000);
      fgs[igs]->SetLineColor(kViolet+1+igs);
      fgs[igs]->SetLineStyle(9);
      fgs[igs]->Write();
      fgs[igs]->Draw("same");
    }
  }
  
  const int nld = ff.GetNlandau();
  TF1  *fld[nld];
  if(nld>0&&kDrawDist){
    for (int ild=0; ild<nld; ild++){
      fld[ild] = new TF1(Form("fld%d",ild),"[2]*TMath::Landau(x,[0],[1],0)",0,500);
      fld[ild]->SetParameters(ff.GetLandauPars(ild));
      fld[ild]->SetNpx(2000);
      fld[ild]->SetLineColor(kViolet+1+ild);
      fld[ild]->SetLineStyle(9);
      fld[ild]->Write();
      fld[ild]->Draw("same");
    }
  }
  
  FILE *fBGT;

  TGraph *verr = Efficiencies::VandleSourceErr();

  TF1 *f1 = new TF1("f1",calcResponse,0,800,2);

  int ng=0;

  if(kPrintEn&&gsFit){
    fBGT = fopen("LevelIntensities.txt","w");
    
    map<double,pair<double,double>> GNInfo = ff.GetSpectrumGNInfo();

    for (std::map<double,pair<double,double>>::reverse_iterator itm = GNInfo.rbegin(); itm!=GNInfo.rend(); ++itm){
      double Ex = itm->first;
      double An = itm->second.second;
      double gamE = itm->second.first;
      double En = (Ex-gamE-3620)/1000.;
      double ntof = fN->GetParameter(2*ng);
      double newAn = fN->GetParameter((2*ng+1))*100./gEff->Eval(En);
      double An_err = fN->GetParError((2*ng+1))*100./gEff->Eval(En);
      f1->SetParameters(fN->GetParameter((2*ng)),fN->GetParameter((2*ng+1)));
      double IntAn = f1->Integral(0,600)*100./gEff->Eval(En);

    /*for (std::map<double,pair<double,double>>::iterator itm = GNInfo.begin(); itm!=GNInfo.end(); itm++){
      double Ex = itm->first;
      double An = itm->second.second;
      double gamE = itm->second.first;
      double En = (Ex-gamE-3620)/1000.;
      double newAn = fN->GetParameter(50-(2*ng+1))*100./gEff->Eval(En);
      double An_err = fN->GetParError(50-(2*ng+1))*100./gEff->Eval(En);
      f1->SetParameters(fN->GetParameter(50-(2*ng)),fN->GetParameter(50-(2*ng+1)));
      double IntAn = f1->Integral(0,600)*100./gEff->Eval(En);
       */
      //if(An_err<10000){
      double p_err = An_err/newAn;
      double e_err = verr->Eval(En)/100.;
      An_err = sqrt(pow(p_err,2)+pow(e_err,2))*IntAn;
      printf("%d\t%.2f\t%.2f\t%.0f\t%.2f\t%.2f\t%.3f\t%.3f\t%.3f\n",ng, Ex/1000., ntof, gamE, newAn, IntAn, An_err, p_err, e_err);
      fprintf(fBGT,"%.2f\t%.0f\t%.2f\t%.2f\t%.3f\n",Ex/1000., gamE, newAn, IntAn, An_err);
      //}
      ng++;
    }

    int nf = fN->GetNpar()/2;
    int ni = ff.GetNGammaNuStates();

    for (int i=ni;i<nf;i++){
      double En = 0.5*mn/pow(c,2)*pow(100./fN->GetParameter(2*i),2);
      double An = fN->GetParameter(2*i+1)*100./gEff->Eval(En);
      double An_err = fN->GetParError(2*i+1)*100./gEff->Eval(En);
      f1->SetParameters(fN->GetParameter(2*i),fN->GetParameter(2*i+1));
      double IntAn = f1->Integral(0,600)*100./gEff->Eval(En);
      
      if(An_err<10000){
      double p_err = An_err/An;
      double e_err = verr->Eval(En)/100.;
      An_err = sqrt(pow(p_err,2)+pow(e_err,2))*IntAn;
      printf("%.2f\t%.2f\t%.2f\t%.3f\t%.3f\t%.3f\n",En+3.62,En,IntAn,An_err,p_err,e_err);
      //fprintf(fBGT,"%.2f\t%.2f\t%.2f\t%.3f\n",En+3.62,En,IntAn,IntAn*p_err);
      fprintf(fBGT,"%.2f\t%.2f\t%.2f\t%.3f\n",En+3.62,En,IntAn,An_err);
      }
    }
    fclose(fBGT);
  }

  int nNuInput = ff.GetNFuncInput();
  if(kDrawNFuncs&&gsFit) PlotIndividualFuncs(fN, nNuInput, ng);
  c1->Write();

  fOut->Close();
  fIn->Close();
  return 0;
}
