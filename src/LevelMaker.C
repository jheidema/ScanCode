#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include <vector>

std::map<string,double> istate;
std::map<string,double> offsets;
std::vector< pair<string, double> > stInfo;



void LevelMaker(const char *fileName){
    vector<string> stLabels;
    stLabels.push_back("134In");
    stLabels.push_back("134Sn");
    stLabels.push_back("133Sn");

    offsets.insert(std::make_pair("134In",14.8));
    offsets.insert(std::make_pair("134Sn",0.0));
    offsets.insert(std::make_pair("133Sn",3.62));

    istate.insert(std::make_pair("134In",0));
    istate.insert(std::make_pair("134Sn",1.5));
    istate.insert(std::make_pair("133Sn",3));


    ifstream fin(fileName);

    string iName;
    double Ex, Etot;

    while(true){
        fin >> iName >> Ex;
        //Etot = Ex + offsets[iName];
        stInfo.push_back(std::make_pair(iName,Ex));
        if(fin.eof()) break;
    }

    Int_t nStates = stInfo.size();
    TLine *ls[nStates];
    double ymax=0;
    for (int i=0;i<nStates;i++){
        string state = stInfo.at(i).first;
        Etot = stInfo.at(i).second + offsets[stInfo.at(i).first];
        if(Etot>ymax) ymax = Etot;        
        ls[i] = new TLine(istate[state],Etot,istate[state]+1,Etot);
        if(stInfo.at(i).second==0.0) {ls[i]->SetLineWidth(3);  ls[i]->SetLineColor(kBlack);}
        else{ls[i]->SetLineWidth(2); ls[i]->SetLineStyle(9);  ls[i]->SetLineColor(kBlue);}
    }

    double xmax = istate[stLabels.at(stLabels.size()-1)]+1;
    

    TCanvas *c1 =  new TCanvas();
    TPad *p1 = new TPad("p1","",0,0,1,1);
    p1->Draw();
    p1->cd();
    gPad->DrawFrame(0,0,xmax,ymax+5);
    gPad->GetFrame()->SetLineWidth(0);
    TH1F *hh = (TH1F*)p1->GetPrimitive("hframe");
    hh->GetXaxis()->SetLabelColor(kWhite);
    //p1->GetXaxis()->SetLabelColor(kWhite);    
    TPaveText *pt[3];
    for (int i=0;i<3;i++){pt[i] = new TPaveText(istate[stLabels.at(i)],-0.5,istate[stLabels.at(i)]+1,-2,"NB"); pt[i]->AddText(stLabels.at(i).c_str());}
    /* pt[0]->AddText("134In");
    pt[1]->AddText("134Sn");
    pt[2]->AddText("133Sn"); */

    ls[0]->Draw();
    pt[0]->Draw("same"); pt[1]->Draw("same"); pt[2]->Draw("same");
    for (int i=1;i<nStates;i++){
        ls[i]->Draw("same");
    }

 return;
}