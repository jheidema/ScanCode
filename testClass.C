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
   scanTree->Branch("En",&En,"En/D");
   scanTree->Branch("barNum",&barNum,"barNum/I");
   scanTree->Branch("Qdc",&Qdc,"Qdc/D");
   scanTree->Branch("sNum",&sNum,"sNum/I");
   scanTree->Branch("sTime",&sTime,"sTime/D");
   scanTree->Branch("sQdc",&sQdc,"sQdc/D");
   scanTree->Branch("cloverE",&cloverE);
   scanTree->Branch("cloverNum",&cloverNum);
   scanTree->Branch("g1560",&g1560,"g1560/O"); 
   
   Double_t prs[6] = {6611.09,2.5873,0.498726,-0.000406763,9.13781e-07,1.09327e-09};
   SetParameters(prs);
   Bool_t gEvent=false;
   Long64_t nentries = fReader.GetEntries(true);

   while(fReader.Next()){
     Bool_t goodEvent = false;
     g1560=false;
     nEntry=0;
     if(nEvent%5000==0) cout<<"\rComputing Entry " << nEvent << " of " << nentries << flush;
     
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
  /* Bool_t goodEvent = false;
  g1560=false;
  if(entry!=-1) GetEntry(entry);
  CopyRootStruc(); */

  if (iv<0) return false;
  /* if(f_vandle_vec_.GetSize()>0){
     goodEvent = true;
     for (int iv = 0; iv < f_vandle_vec_.GetSize(); iv++){ */
         double oToF = f_vandle_vec_.At(iv).corTof;
         Qdc = f_vandle_vec_.At(iv).qdc;
         if(oToF>-100&&oToF<1000){
          nToF = WCorrToF(oToF,Qdc);
          En = CalculateEnergy(nToF);
          barNum = f_vandle_vec_.At(iv).barNum;         
          
          //beta start Info
          sNum  = f_vandle_vec_.At(iv).sNum;
          sTime = f_vandle_vec_.At(iv).sTime;
          sQdc  = f_vandle_vec_.At(iv).sQdc;
          return true;
         } else {
            ZeroVandStruc(); return false;
         }
     /*} 
  }
   ZeroRootStruc(); */
  //return goodEvent;
}