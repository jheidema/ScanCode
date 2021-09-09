#include <stdio.h>

#include "TGraphAsymmErrors.h"
#include "TGraph.h"

int nstates=3;
int minJ=5, maxJ=7;
int lstyle[3] = {1,9,7,10};
int color[10] = {2,3,4,5,6,7,8,9,11,12};
int mstyle[3] = {24,25,26};
//int mstyle[3] = {53,54,55};
int mcolor[3] = {1,1,1};


void hfplot(void){
gStyle->SetEndErrorSize(6);

// Load Experimental Data
TGraphAsymmErrors *gdata[3];
for (int i=0; i<nstates;i++){
    gdata[i] = new TGraphAsymmErrors(Form("../input/data_%d.tsv",i),"%lg %lg %lg %lg");
    gdata[i]->SetLineColor(mcolor[i]);
    gdata[i]->SetMarkerColor(mcolor[i]);
    gdata[i]->SetMarkerStyle(mstyle[i]);
    gdata[i]->SetMarkerSize(1.5);
    gdata[i]->SetLineWidth(2);
}

// Load BEOH Predictions

TGraph *hfg[10][nstates];

ifstream fin;
fin.open("../input/HF_GT.tsv");
if(!fin.is_open()){
    cout << "HF File not opened.\n"; 
    return 1;
}
//fin.ignore(256,'##');

int j;
double jd, ex, pn, p7, p3, p9, p5;

//J	    Energy	Pn	    7/2	    3/2	    9/2	    5/2
fin >> jd >> ex >> pn >> p7 >> p3 >> p9 >> p5 ;
j=(int)jd;
int j_init = j;
int ij = 0;
int nj = 1;
int j_old=j;

while(fin.good()){
    //cout << j << "\t" << ex << "\t" << pn << "\t" << p7 << "\t" << p3 << "\t" << p9 << "\t" << p5 << "\t" << endl;
    if (j_old!=j||(j==0&&ij==0)){
        for (int k=0;k<nstates;k++) {hfg[j][k] = new TGraph(); hfg[j][k]->SetName(Form("hfg%d_%d",j,k));}
        //for (int k=0;k<nstates;k++) {cout << "Initializing graph: hfg" << j << "_"<< k <<"\n" ; hfg[j][k] = new TGraph(); hfg[j][k]->SetName(Form("hfg%d_%d",j,k));}
        nj++;
        ij=0;
    }
    double ptot = p7+p3+p9;
    hfg[j][0]->SetPoint(ij,ex,p7/ptot);
    hfg[j][1]->SetPoint(ij,ex,p3/ptot);
    hfg[j][2]->SetPoint(ij,ex,p9/ptot);
    ij++;
    j_old = j;
  
    fin >> jd >> ex >> pn >> p7 >> p3 >> p9 >> p5 ;
    j=(int)jd;
}


//Plot everything

TCanvas *c1 = new TCanvas("c1","c1",800,600);

for(int i=minJ; i<=maxJ; i++){
    int ii=i-minJ;

    for(int k=0;k<nstates;k++){
        hfg[i][k]->SetLineColor(color[ii]);
        hfg[i][k]->SetMarkerColor(color[ii]);
        hfg[i][k]->SetLineWidth(3);
        hfg[i][k]->SetLineStyle(lstyle[k]);

        if(i==minJ&&k==0){
            hfg[i][k]->Draw();
            hfg[i][k]->GetYaxis()->SetRangeUser(0.0,1.0);
            hfg[i][k]->GetXaxis()->SetRangeUser(5.5,9.5);
        } else
            hfg[i][k]->Draw("same");
    }
    //cout << i << endl;
}

for (int i=0; i<nstates;i++){
    gdata[i]->Draw("P same");
}

TLegend *lhf = new TLegend(0.7,0.7,0.9,0.9);
for(int i=0;i<maxJ-minJ+1;i++)
    for(int k=0;k<nstates;k++)
        lhf->AddEntry(hfg[i+minJ][k],Form("HF_J%d_%d",i,k));
lhf->Draw("same");

TLegend *ld = new TLegend(0.6,0.7,0.7,0.9);
for (int i=0;i<nstates;i++)
    ld->AddEntry(gdata[i],Form("State %d",i),"lp");

ld->Draw("same");

c1->SaveAs("comp.png");
c1->SaveAs("comp.pdf");

return;
}