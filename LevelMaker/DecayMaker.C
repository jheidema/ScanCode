#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include <vector>

std::map<string,double> istate;
std::map<string,double> offsets;
std::vector< pair<string, double> > stInfo;
std::vector< int > kVec;


void LevelMaker(const char *fileName){
    vector<string> stLabels;
    stLabels.push_back("134In");
    stLabels.push_back("134Sn");
    stLabels.push_back("133Sn");
    stLabels.push_back("132Sn");

    offsets.insert(std::make_pair("134In",14.8));
    offsets.insert(std::make_pair("134Sn",0.0));
    offsets.insert(std::make_pair("133Sn",3.62));
    offsets.insert(std::make_pair("132Sn",6.02));

    istate.insert(std::make_pair("134In",0));
    istate.insert(std::make_pair("134Sn",1.5));
    istate.insert(std::make_pair("133Sn",3));
    istate.insert(std::make_pair("132Sn",4.5));


    ifstream fin(fileName);

    string iName;
    double Ex, Etot;
    int iK;
    while(true){
        fin >> iName >> Ex >> iK;
        //Etot = Ex + offsets[iName];
        stInfo.push_back(std::make_pair(iName,Ex));
        kVec.push_back(iK);
        if(fin.eof()) break;
    }

    Int_t nIso = stLabels.size();
    
    Int_t nStates = stInfo.size();
    TLine *ls[nStates];
    TLine *lsn[4];
    double ymax=0;
    int isn=0;
    for (int i=0;i<nStates;i++){
        string state = stInfo.at(i).first;
        Etot = stInfo.at(i).second + offsets[stInfo.at(i).first];
        if(Etot>ymax) ymax = Etot;        
        ls[i] = new TLine(istate[state],Etot,istate[state]+0.5,Etot);
        if(stInfo.at(i).second==0.0) {
            ls[i]->SetLineWidth(4);  ls[i]->SetLineColor(kBlack);
            if(istate[state]!=0) {
                lsn[isn]=new TLine(0,Etot,istate[state],Etot);
                lsn[isn]->SetLineWidth(2); lsn[isn]->SetLineStyle(2); lsn[isn]->SetLineColor(kBlack);
                isn++;
            }
        }
        else if(kVec.at(i)==1) {ls[i]->SetLineWidth(1); ls[i]->SetLineStyle(1);  ls[i]->SetLineColor(kBlue);}
        else{ls[i]->SetLineWidth(1); ls[i]->SetLineStyle(1);  ls[i]->SetLineColor(kBlack);}
    }

    double xmax = istate[stLabels.at(stLabels.size()-1)]+1;
    
    if(!gROOT->GetListOfCanvases()->Contains("c1")){
    TCanvas *c1 =  new TCanvas();
    }
    TPad *p1 = new TPad("p1","",0,0,1,1);
    p1->Draw();
    p1->cd();
    gPad->DrawFrame(0,0,xmax,ymax+2);
    gPad->GetFrame()->SetLineWidth(0);
    TH1F *hh = (TH1F*)p1->GetPrimitive("hframe");
    hh->GetXaxis()->SetLabelColor(kWhite);
    hh->GetXaxis()->SetNdivisions(5,2,0);
    hh->GetYaxis()->SetTitle("E* (MeV)");
    //p1->GetXaxis()->SetLabelColor(kWhite);    
    TPaveText *pt[nIso];
    for (int i=0;i<nIso;i++){
	    pt[i] = new TPaveText(istate[stLabels.at(i)],-0.5,istate[stLabels.at(i)]+1,-2,"NB"); 
	    pt[i]->AddText(stLabels.at(i).c_str());
        pt[i]->SetTextSize(0.05);
    	}

    /* pt[0]->AddText("134In");
    pt[1]->AddText("134Sn");
    pt[2]->AddText("133Sn"); */

    ls[0]->Draw();
    pt[0]->Draw("same"); pt[1]->Draw("same"); 
    pt[2]->Draw("same"); pt[3]->Draw("same");

    for (int i=1;i<nStates;i++)
        ls[i]->Draw("same");
    for (int i=1;i<isn;i++)
        lsn[i]->Draw("same");

 return;
}
