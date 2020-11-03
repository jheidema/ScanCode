
#include "../include/Resolution.hpp"

void PlotRes(double dT = 1.0, double pL = 100.0, double dL = 3.0){

	TF1 *v1 = new TF1("v1",EResolution,0.05,10,3);
	v1->SetNpx(1000);
	v1->SetParameters(dL,dT,pL);

	TF1 *f[6];
	double xth[6] = {0.6,0.5,0.4,0.3,0.2,0.1};
	for (int i=0;i<6;i++){
		f[i] = new TF1(Form("f%d",i),EResolution,0.05,10,3);
		f[i]->SetNpx(1000);
		f[i]->SetParameters(xth[i],0.5,100.0);
	}

	TCanvas *c1 = new TCanvas();
	v1->Draw();
	for (int i=0;i<6;i++) f[i]->Draw("same");


	TF1 *v2 = new TF1("v2",EResolution,0.05,10,3);
	v2->SetNpx(1000);
	v2->SetParameters(3.0,1.0,50.0);

	TF1 *n1 = new TF1("n1",EResolution,0.05,10,3);
	n1->SetNpx(1000);
	n1->SetParameters(0.5,0.5,100.0);

	TF1 *n2 = new TF1("n2",EResolution,0.05,10,3);
	n2->SetNpx(1000);
	n2->SetParameters(0.5,0.5,50.0);


	TCanvas *c2 = new TCanvas();
	v2->Draw();
	v1->DrawClone("same");
	n1->Draw("same");
	n2->Draw("same");

	return;
}
