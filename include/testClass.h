//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep 25 12:41:40 2019 by ROOT version 6.14/06
// from TTree PixTree/Pixie Event Tree
// found on file: fullfile_scan0.root
//////////////////////////////////////////////////////////

#ifndef testClass_h
#define testClass_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TF1.h>

// Header file for the classes stored in the TTree if any.
#include "ProcessorRootStruc.hpp"
#include "TObject.h"
#include <vector>
#include <iostream>

#include "dummyFuncs.hpp"

using namespace std;

Double_t mN = 939.57; //MeV/c^2

class testClass {
public :
   TTreeReader     fReader;
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   TTreeReaderArray<processor_struct::CLOVERS>     f_clover_vec_ = {fReader, "clover_vec_"};
   TTreeReaderArray<processor_struct::DOUBLEBETA>  f_doublebeta_vec_ = {fReader,"doublebeta_vec_"};
   TTreeReaderArray<processor_struct::GAMMASCINT>  f_gamma_scint_vec_ = {fReader,"gamma_scint_vec_"};
   TTreeReaderArray<processor_struct::LOGIC>       f_logic_vec_ = {fReader,"logic_vec_"};
   TTreeReaderArray<processor_struct::PSPMT>       f_pspmt_vec_ = {fReader,"pspmt_vec_"};
   TTreeReaderArray<processor_struct::ROOTDEV>     f_root_dev_vec_ = {fReader,"root_dev_vec_"};
   TTreeReaderArray<processor_struct::VANDLES>     f_vandle_vec_ = {fReader,"vandle_vec_"};
   TTreeReaderArray<processor_struct::NEXTS>       f_next_vec_ = {fReader,"next_vec_"};

   TTreeReaderValue<UInt_t>      fUniqueID    = {fReader, "fUniqueID"};
   TTreeReaderValue<UInt_t>      fBits        = {fReader, "fBits"};
   TTreeReaderValue<ULong64_t>   externalTS1  = {fReader,"externalTS1"};
   TTreeReaderValue<ULong64_t>   externalTS2  = {fReader,"externalTS2"};
   TTreeReaderValue<Double_t>    eventNum     = {fReader,"eventNum"};
   TTreeReaderValue<string>      fileName     = {fReader,"fileName"};
   
   TF1 *fCorr;
   TF1 *fQuench;
   TF1 *fQDC;
   Double_t *pars;
   
   Long64_t    nEvent = 0;
   Int_t       nEntry;
   Bool_t      g1560;
   Bool_t      g853;
   Bool_t      g2004;
   Bool_t      gBirk;
   Double_t    nToF, oToF;
   Double_t    En;
   Double_t    Qdc;
   Int_t       barNum;
   Int_t       sNum;
   Double_t    sTime;
   Double_t    sQdc;
   std::vector<Double_t> cloverE;
   std::vector<Int_t> cloverNum;

   //Output Tree 
   //TTree *scanTree = new TTree("T","ScanOut Tree");

   testClass(TTree *tree=0);
   virtual ~testClass();
   virtual Int_t     Cut(Long64_t entry);
   virtual Int_t     GetEntry(Long64_t entry);
   //virtual Long64_t LoadTree(Long64_t entry);
   virtual Bool_t    Analyze(Int_t iv =-1);
   virtual void      Init(TTree *tree);
   virtual void      Loop();
   virtual Bool_t    Notify();
   virtual void      Show(Long64_t entry = -1);
   //virtual Bool_t    FillRootStruc();
   virtual void      ZeroRootStruc();
   virtual void      ZeroVandStruc();
   virtual void      CopyRootStruc();
   virtual void      SetParameters(Double_t *p1, Double_t *p2, Double_t *p3); 
   virtual Double_t  WCorrToF(Double_t iToF, Double_t qdc);
   virtual Double_t  CalculateEnergy(Double_t tof);
};

#endif

#ifdef testClass_cxx
testClass::testClass(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
 /*   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("fullfile_scan0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("fullfile_scan0.root");
      }
      f->GetObject("PixTree",tree);

   } */
   Init(tree);
}

testClass::~testClass()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
   fCorr->Delete();
   fQuench->Delete();
   fQDC->Delete();
}

Int_t testClass::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fReader.SetLocalEntry(entry);
}


void testClass::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fReader.SetTree(tree);

   fCorr = new TF1("fCorr",tofWcorr,0,35000,6);
   fQuench = new TF1("fQuench",MadeyQ,0,20,4);
   fQDC = new TF1("fQDC","[0]+[1]*x",0,20);
   Notify();
}

Bool_t testClass::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

Int_t testClass::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
void testClass::Show(Long64_t entry){
   
   GetEntry(entry);

   Int_t vSize = f_vandle_vec_.GetSize();
   Int_t cSize = f_clover_vec_.GetSize();

   if(vSize>2) std::cout << "Vandle events: " << vSize << ", Clover Events: " << cSize << endl;

}
void testClass::ZeroRootStruc(){
  ZeroVandStruc();
  cloverE.clear();
  cloverNum.clear();
  g1560 = false;
  g853 = false;
  g2004 = false;
  gBirk =  false;
  nEntry = 0;
}
void testClass::ZeroVandStruc(){
  nToF   = -9999;
  oToF   = -9999;
  En     = -9999;
  barNum = -9999;
  Qdc    = -9999;
  sNum   = -9999;
  sTime  = -9999;
  sQdc   = -9999;
}

void testClass::CopyRootStruc(){
   //Copying Important Clover information
   for (int ic=0; ic<f_clover_vec_.GetSize(); ic++){
      double cEnergy = f_clover_vec_.At(ic).energy;
      cloverE.push_back(cEnergy);
      cloverNum.push_back(f_clover_vec_.At(ic).detNum);
      if( cEnergy>1558 && cEnergy<1564 ) g1560=true;
      if( cEnergy>850 && cEnergy<857) g853=true;
      if( cEnergy>2002 && cEnergy<2005) g2004=true;
    }

   //Copy Event Number for nToF multiplicity.
   Double_t nEvent = *eventNum;
}

void testClass::SetParameters(Double_t *p1, Double_t *p2, Double_t *p3){
  fCorr->SetParameters(p1);
  fQuench->SetParameters(p2);
  fQDC->SetParameters(p3);
}

Double_t testClass::WCorrToF(Double_t iToF, Double_t qdc){
  if(iToF<-100) return -9999;
  double corr = fCorr->Eval(qdc);
  double newToF = iToF-corr+3.34;
  if (newToF<-200&&newToF>=1000) return -9999;
  else return newToF;
}

Double_t testClass::CalculateEnergy(Double_t tof){
  double energy;
  if (tof>0){
    if(tof>4) 
      energy = 0.5 * mN * pow(100./tof/29.98,2); // MeV/c^2
    else 
      energy = -500;
  }
  else energy = -9999;
  return energy;
}

#endif // #ifdef testClass_cxx
