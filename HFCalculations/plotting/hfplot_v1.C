#include <stdio.h>

#include "TGraphErrors.h"
#include "TGraph.h"

int nstates=3;
int minJ=5, maxJ=8;
void hfplot(void){

// Load Experimental Data
TGraphErrors *gdata[3];
for (int i=0; i<nstates;i++){
    gdata[i] = new TGraphErrors(Form("../input/data_%d.tsv",i),"%lg %lg %lg");
    gdata[i]->SetLineColor(1);
    gdata[i]->SetMarkerColor(1);
    gdata[i]->SetMarkerStyle(20+i);
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

int lstyle[3] = {1,9,7};
TCanvas *c1 = new TCanvas();
hfg[minJ][0]->SetLineColor(1);
hfg[minJ][0]->SetLineWidth(4);
hfg[minJ][0]->SetLineStyle(lstyle[0]);
hfg[minJ][0]->Draw();
hfg[minJ][0]->GetYaxis()->SetRangeUser(0.0,1.0);
for(int k=1;k<nstates;k++){
        hfg[minJ][k]->SetLineColor(1);
        hfg[minJ][k]->SetLineWidth(4);
        hfg[minJ][k]->SetLineStyle(lstyle[k]);

        hfg[minJ][k]->Draw("same");
}

for(int i=minJ+1; i<=maxJ; i++){
    for(int k=0;k<nstates;k++){
        hfg[i][k]->SetLineColor(i-minJ+1);
        hfg[i][k]->SetLineWidth(4);
        hfg[i][k]->SetLineStyle(lstyle[k]);

        hfg[i][k]->Draw("same");
    }
    //cout << i << endl;
}

for (int i=0; i<nstates;i++){
    gdata[i]->Draw("P same");
}

}