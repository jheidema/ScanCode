#include <stdio.h>

#include "TGraphAsymmErrors.h"
#include "TGraph.h"

const int nstates=3;
double maxE=8.5, minE=6;
int outJ=6;
int minJ=5, maxJ=8;
int lwidth=2;
int lstyle[4] = {1,9,7,2};
//int color[9] = {2,kGreen+2,4,6,7,8,9,11,12};
//int color[12] = {kCyan+3,kCyan+2,kCyan+1,kCyan,kGreen+3,kGreen+2,kGreen+1,kGreen,kRed+3,kRed+2,kRed+1,kRed};
//int color[12] = {kRed+3,kRed+2,kRed+1,kRed,kRed+3,kRed+2,kRed+1,kRed,kRed+3,kRed+2,kRed+1,kRed};
int color[12] = {kViolet-1,kMagenta,kRed,kOrange,kViolet-1,kMagenta,kRed,kOrange,kViolet-1,kMagenta,kRed,kOrange};

int mstyle[nstates] = {20,21,22}; // filled markers
//int mstyle[nstates] = {24,25,26}; // hollow markers
//int mstyle[nstates] = {53,54,55}; // hollow markers, thick line
int mcolor[nstates] = {1,1,1};
int mfill[nstates] = {kCyan+4,kCyan-4,kCyan-2};
int spin[nstates] = {7,3,9};

double calcTOF(double E){
    return 100.0/29.98*sqrt(938.9/(2*E));
}

double calcErr(double E){
    double tof = calcTOF(E);
    return sqrt(pow(2*1.0/tof,2)+pow(2*0.03/1,2))*E;
}


/// Main Function ///
void hfplot(void){

gStyle->SetEndErrorSize(5);

// Load Experimental Data
TGraphAsymmErrors *gdata[3];
for (int i=0; i<nstates;i++){
    //gdata[i] = new TGraphAsymmErrors(Form("../input/data_%d.tsv",i),"%lg %lg %lg %lg %lg %lg");
    gdata[i] = new TGraphAsymmErrors(Form("../input/data_%d.tsv",i),"%lg %lg %lg %lg");
    
    gdata[i]->SetLineColor(mcolor[i]);
    gdata[i]->SetMarkerColor(mcolor[i]);
    gdata[i]->SetMarkerStyle(mstyle[i]);
    gdata[i]->SetFillColorAlpha(mfill[i],0.75);
    gdata[i]->SetMarkerSize(0.85);
    gdata[i]->SetLineWidth(lwidth-1);

    for(int j=0;j<gdata[i]->GetN();j++) {
        //cout << gdata[i]->GetPointX(j) << " " << calcErr(-1.*(gdata[i]->GetPointX(j)+3.62+1.561)) << endl;
        double err = calcErr(-1.*(gdata[i]->GetPointX(j)+3.62+1.561));
        gdata[i]->SetPointEXhigh(j,err/2.);
        gdata[i]->SetPointEXlow(j,err/2.);
    }
}

// Load BEOH Predictions

TGraph *ghf[10][nstates];
TGraph *gout[nstates];

ifstream fin;
//fin.open("../input/HF_GT.tsv");
fin.open("../input/koning/hf_koning.tsv");
//fin.open("../input/becchetti/hf_becchetti.tsv");
//fin.open("../input/kunieda/hf_kunieda.tsv");

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
        if(j!=0){
            ghf[j_old][0]->SetPoint(ij,(ghf[j_old][0]->GetPointX(ij-1)),0);
            ghf[j_old][1]->SetPoint(ij,(ghf[j_old][1]->GetPointX(ij-1)),0);
            ghf[j_old][2]->SetPoint(ij,(ghf[j_old][2]->GetPointX(ij-1)),0);
        }
        for (int k=0;k<nstates;k++) {ghf[j][k] = new TGraph(); ghf[j][k]->SetName(Form("ghf%d_%d",j,k));}
        //for (int k=0;k<nstates;k++) {cout << "Initializing graph: ghf" << j << "_"<< k <<"\n" ; ghf[j][k] = new TGraph(); ghf[j][k]->SetName(Form("ghf%d_%d",j,k));}
        nj++;
        ij=0;
        ghf[j][0]->SetPoint(ij,-ex,0);
        ghf[j][1]->SetPoint(ij,-ex,0);
        ghf[j][2]->SetPoint(ij,-ex,0);
        ij++;

        
    }
    double ptot = p7+p3+p9;
    ghf[j][0]->SetPoint(ij,-ex,p7/ptot);
    ghf[j][1]->SetPoint(ij,-ex,p3/ptot);
    ghf[j][2]->SetPoint(ij,-ex,p9/ptot);
    ij++;
    j_old = j;
  
    fin >> jd >> ex >> pn >> p7 >> p3 >> p9 >> p5 ;
    j=(int)jd;
    
}

ghf[j_old][0]->SetPoint(ij,(ghf[j_old][0]->GetPointX(ij-1)),0);
ghf[j_old][1]->SetPoint(ij,(ghf[j_old][1]->GetPointX(ij-1)),0);
ghf[j_old][2]->SetPoint(ij,(ghf[j_old][2]->GetPointX(ij-1)),0);

string sin;
bool kShade=false;
cout << "Shade Histogram? (y/n) :" << endl;
getline(cin,sin);
//cin >> sin;

if(sin=="y"){
    kShade=true;
    gROOT->SetBatch(true);
}

//Plot everything

TFile *outF = new TFile("hfhists.root","RECREATE");

TCanvas *c1 = new TCanvas("c1","c1",800,600);
c1->Divide(1,3);

for(int i=minJ; i<=maxJ; i++){
    int ii=i-minJ;

    for(int k=0;k<nstates;k++){
        c1->cd(3-k);
        ghf[i][k]->SetLineColor(color[ii+4*k]);
        ghf[i][k]->SetMarkerColor(color[ii+4*k]);
        ghf[i][k]->SetLineWidth(lwidth);
        ghf[i][k]->GetXaxis()->SetLimits(-maxE,-minE);
        ghf[i][k]->SetLineStyle(lstyle[ii]);

        
        if(i==outJ){
            gout[k] = (TGraph*)ghf[i][k]->Clone(Form("h%d",k));
            ghf[i][k]->Write();
        }

        if(i==minJ){

            ghf[i][k]->GetXaxis()->SetTitleSize(0.08);
            ghf[i][k]->GetXaxis()->SetTitleOffset(0.75);
            ghf[i][k]->GetXaxis()->SetLabelSize(0.07);
            ghf[i][k]->GetXaxis()->SetNdivisions(508);
            ghf[i][k]->GetXaxis()->SetTickSize(0.05);
            ghf[i][k]->GetXaxis()->SetLabelColor(kWhite);
            ghf[i][k]->GetXaxis()->SetRangeUser(-maxE,-minE);
            
            ghf[i][k]->GetYaxis()->SetLimits(0.0,1.0);
            ghf[i][k]->GetYaxis()->SetRangeUser(0.0,1.0);
            ghf[i][k]->GetYaxis()->SetNdivisions(505);
            ghf[i][k]->GetYaxis()->SetLabelSize(0.06);
            ghf[i][k]->GetYaxis()->SetLabelOffset(0.015);
            
            for(int ij=1;ij<=6;ij++) ghf[i][k]->GetYaxis()->ChangeLabel(ij,90,-1,12,-1,-1);
            
            //ghf[i][k]->GetYaxis()->CenterLabels(true);
            //for(int ij=1;ij<=6;ij++) ghf[i][k]->GetYaxis()->ChangeLabel(ij,90,-1,1,-1,-1,Form("%.1f   ",(ij-1)*2/10.));


            if(kShade){ 
                ghf[i][k]->SetFillColorAlpha(color[ii+4*k],0.3);
                ghf[i][k]->Draw("APLF");
            }
            else 
                ghf[i][k]->Draw();
            
            
            
            
        } else
            ghf[i][k]->Draw("same");
    }

}



int np=gout[0]->GetN();

for(int i=1;i<np-1;i++){    
    double yval = gout[1]->GetPointY(i);
    gout[1]->SetPointY(i,gout[0]->GetPointY(i)+yval);
    gout[2]->SetPointY(i,1);
}
for(int i=0;i<nstates;i++)
    gout[i]->Write();

outF->Close();



for (int i=0; i<nstates;i++){
    c1->cd(nstates-i);
    //gdata[i]->SetFillColorAlpha(kBlack,0.65);
    gdata[i]->Draw("P5 same");
    gPad->RedrawAxis();
}

TLegend *lhf[3];// = new TLegend(0.7,0.7,0.9,0.9);
for(int i=0;i<maxJ-minJ+1;i++)
    for(int k=0;k<nstates;k++){
        if(i==0){
            lhf[k] = new TLegend(0.91,0.6,0.99,0.9);
            lhf[k]->SetNColumns(2);
            lhf[k]->SetHeader(Form("%d/2",spin[k]),"C");
        }
        
        lhf[k]->AddEntry(ghf[i+minJ][k],Form("J=%d",i+minJ),"l");
        
        if(i==maxJ-minJ){
            c1->cd(nstates-k);
            lhf[k]->Draw("same");
        }
    }


//TLegend *ld = new TLegend(0.6,0.7,0.9,0.8);
//ld->SetNColumns(3);
////ld->SetColumnSeparation(-0.2);
//for (int i=0;i<nstates;i++)
//    ld->AddEntry(gdata[i],Form("%d/2",spin[i]),"pf");
//    c1->cd(nstates-i);
//    //lhf[i]->Draw("same");
//}
//ld->Draw("same");

c1->SaveAs("comp.png");
c1->SaveAs("comp.pdf");

if(kShade) gApplication->Terminate();
//return;
}