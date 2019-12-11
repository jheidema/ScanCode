#define testClass_cxx
#include "testClass.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>


void testClass::Loop()
{

   if (fChain == 0) return;
  TFile *outF = new TFile("testOut.root","RECREATE");
   
   TTree *scanTree = new TTree("T","ScanOut Tree");
   scanTree->Branch("event",&nEvent,"event/D");
   scanTree->Branch("nEntry",&nEntry,"nEntry/I");
   scanTree->Branch("nToF",&nToF,"nToF/D");
   scanTree->Branch("oToF",&oToF,"oToF/D");
   scanTree->Branch("En",&En,"En/D");
   scanTree->Branch("barNum",&barNum,"barNum/I");
   scanTree->Branch("Qdc",&Qdc,"Qdc/D");
   scanTree->Branch("sNum",&sNum,"sNum/I");
   scanTree->Branch("sTime",&sTime,"sTime/D");
   scanTree->Branch("sQdc",&sQdc,"sQdc/D");
   scanTree->Branch("cloverE",&cloverE);
   scanTree->Branch("cloverNum",&cloverNum);
   scanTree->Branch("g1560",&g1560,"g1560/O"); 
   scanTree->Branch("g853",&g853,"g853/O"); 
   scanTree->Branch("g2004",&g2004,"g2004/O");
   scanTree->Branch("gBirk",&gBirk,"gBirk/O");

   Double_t corr_prs[6] = {6611.09,2.5873,0.498726,-0.000406763,9.13781e-07,1.09327e-09};
   Double_t Mad_prs[4] = {0.95,8.0,0.1,0.90};
   Double_t cal_prs[2] = {364.405,13.5098};  //QDCcutoff Fit to 49K data
   //Double_t cal_prs[2] = {1259.0,8.456}; // Fit to 60Co data
   SetParameters(corr_prs,Mad_prs,cal_prs);
   Bool_t gEvent=false;
   Long64_t nentries = fReader.GetEntries(true);

   while(fReader.Next()){
      Bool_t goodEvent = false;
      if(nEvent%5000==0) cout<<"\rComputing Entry " << nEvent << " of " << nentries << flush;
      gBirk = false;
     CopyRootStruc();
      
      if(f_vandle_vec_.GetSize()>0){
       for (int iv = 0; iv < f_vandle_vec_.GetSize(); iv++){
         goodEvent = Analyze(iv);
         if(goodEvent) {nEntry++; scanTree->Fill();}
         ZeroVandStruc();
       }
      }
     ZeroRootStruc();
     nEvent++;
     //if (nEvent==1000) break;
   }
   cout<<endl;
   scanTree->Write();
   outF->Close();
}

Bool_t testClass::Analyze(Int_t iv){

  if (iv<0) return false;
  gBirk=true;
  oToF = f_vandle_vec_.At(iv).corTof;
  Qdc = f_vandle_vec_.At(iv).qdc;
  barNum = f_vandle_vec_.At(iv).barNum;         
  if(oToF>-150&&oToF<1000){
   nToF = WCorrToF(oToF,Qdc);
   En = CalculateEnergy(nToF);

   Double_t el_eq = fQuench->Eval(En);
   Double_t maxQdc = 2.*fQDC->Eval(el_eq*1000.);
   if(Qdc>maxQdc) gBirk = false;
   //beta start Info
   sNum  = f_vandle_vec_.At(iv).sNum;
   sTime = f_vandle_vec_.At(iv).sTime;
   sQdc  = f_vandle_vec_.At(iv).sQdc;
   return true;
  } else {
     ZeroVandStruc(); return false;
  }
  //return goodEvent;
}