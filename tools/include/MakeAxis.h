#include "TGaxis.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH1F.h"

TGaxis *MakeAxis(double xmin, double xmax, double ymin, double ymax){

  TGaxis *eAxis = new TGaxis();

  double lo = 0.5*939.6*pow(100.0/xmin/29.98,2);
  double hi = 0.5*939.6*pow(100.0/xmax/29.98,2);

  cout << "Range: " << lo << " to " << hi << endl;

  if (ymin==ymax) {
	  TF1 *nEnergy = new TF1("nEnergy","sqrt(939.6/(2*x))*100.",lo,hi); 
	  //TF1 *nEnergy = new TF1("nEnergy","1/(x*x)",lo,hi); 
	  eAxis = new TGaxis(xmin,ymin,xmax,ymax,"nEnergy",50510,"-");
  }//else if (xmin==xmax) {
  //	TF1 *nEnergy = new TF1("nEnergy","0.5*939.6*pow(100.0/x/29.98,2)",ymin,ymax);
	//  eAxis = new TGaxis(xmin,ymin,xmax,ymax,"nEnergy",510,"-");
  //}

  return eAxis;
}

TGaxis* MakeAxis(TH1F *hIn, Bool_t kDraw=false, double offset=0.0){

  TGaxis *eAxis;

  //double xmin = hIn->GetBinLowEdge(hIn->GetXaxis()->GetFirst());
  //double xmax = hIn->GetBinLowEdge(hIn->GetXaxis()->GetLast()) + hIn->GetBinWidth(hIn->GetXaxis()->GetLast());
  double xmin = gPad->GetUxmin();
  double xmax = gPad->GetUxmax();
  double ymax;
  //if(kDraw){ 
  //  ymax = ceil(hIn->GetMaximum()*1.10);
  //  hIn->GetYaxis()->SetRangeUser(0,ymax);
  //}
  //else
    ymax = gPad->GetUymax();
  
  double lo = 0.5*939.6*pow(100.0/xmin/29.98,2)+offset;
  double hi = 0.5*939.6*pow(100.0/xmax/29.98,2)+offset;
  
  TF1 *nEnergy = new TF1("nEnergy",Form("sqrt(939.6/(2*(x-%f)))*100.",offset),lo,hi); 
  //TF1 *nEnergy = new TF1("nEnergy","sqrt(939.6/(2*x))*100.",lo,hi); 
  eAxis = new TGaxis(xmin,ymax,xmax,ymax,"nEnergy",50510,"-");
  eAxis->SetLabelOffset(0.0);

  if(kDraw){
  hIn->Draw();
  eAxis->Draw();
  }

  return eAxis;

}


TGaxis* MakeAxis(TH1D *hIn, Bool_t kDraw=false, double offset=0.0){

  TGaxis *eAxis;

  double xmin = gPad->GetUxmin();
  double xmax = gPad->GetUxmax();
  double ymax;
  
  ymax = gPad->GetUymax();
  
  double lo = 0.5*939.6*pow(100.0/xmin/29.98,2)+offset;
  double hi = 0.5*939.6*pow(100.0/xmax/29.98,2)+offset;
  
  TF1 *nEnergy = new TF1("nEnergy",Form("sqrt(939.6/(2*(x-%f)))*100.",offset),lo,hi); 
  eAxis = new TGaxis(xmin,ymax,xmax,ymax,"nEnergy",50510,"-");
  eAxis->SetLabelOffset(0.0);

  if(kDraw){
  hIn->Draw();
  eAxis->Draw();
  }

  return eAxis;
}