#include <iostream>

#include <vector>
#include "TGraph.h"

void Dist(){

//double Qb = 1990.0;  //keV  Beta-
double Qb = 1440.0; //Beta+
double m_e = 511.0, m_nu = 0.002; //keV
double e = 1.602E-19;
double e0 = 8.85E-12;
double hb = 1.054E-34;
float F,eta,v_e;
double w, nu_w, T_e;
const int h = 10; //eV
std::vector <double> X,Y,nu_Y;
const int nsteps = ((int)Qb-h)/h;
//std::ofstream file;
//file.open("Values.txt");
double prev[2] = {0,0};
double intg[2] = {0,0};

 for (int i=1; i<nsteps+1; i++){
   T_e = i*h;
   v_e = 3.00E8*sqrt(1-1/((T_e/m_e+1)*(T_e/m_e+1)));
//   std::cout << v_e << endl;
   X.push_back(T_e);
   eta = -2*3.1416*49*e*e/(4*3.1416*e0*hb*v_e);
   F = eta/(1-exp(-eta));
//   w = F*sqrt(T_e*T_e+2*T_e*m_e)*sqrt(1-m_nu*m_nu/((Qb-T_e)*(Qb-T_e)))*(Qb-T_e)*(Qb-T_e)*(T_e+m_e)/7.44218e+15;  //B-
//   nu_w = sqrt(1-m_e*m_e/((m_e+Qb-T_e)*(m_e+Qb-T_e)))*(m_e+Qb-T_e)*(m_e+Qb-T_e)*T_e*T_e/2.66618e+15;   // B-
   w = F*sqrt(T_e*T_e+2*T_e*m_e)*sqrt(1-m_nu*m_nu/((Qb-T_e)*(Qb-T_e)))*(Qb-T_e)*(Qb-T_e)*(T_e+m_e)/1.35528e+14;  // B+
   nu_w = sqrt(1-m_e*m_e/((m_e+Qb-T_e)*(m_e+Qb-T_e)))*(m_e+Qb-T_e)*(m_e+Qb-T_e)*T_e*T_e/6.77772e+14;   // B+
//   printf("F: %f, X: %f, Y: %f\n",F,T_e,w);
//   file << T_e << " " << F << std::endl;
   Y.push_back(w);
   nu_Y.push_back(nu_w);
   if (i>1) {intg[0] += 10.0*0.5*(prev[0]+w);intg[1] += 10.0*0.5*(prev[1]+nu_w);}
   prev[0] = w;
   prev[1] = nu_w;
  }
std::cout << intg[0] << "  " << intg[1] << std::endl;
//file.close();
//std::cout << nsteps << " " << X.size() << " " << Y.size() << endl;
TGraph *g1 = new TGraph(nsteps,&(X[0]),&(Y[0]));
TGraph *g2 = new TGraph(nsteps,&(X[0]),&(nu_Y[0]));
g1->SetLineColor(kBlue);
g1->SetLineWidth(2);
g2->SetLineColor(kRed);
g2->SetLineWidth(2);

g1->Draw("APL");
g2->Draw("same");


TLegend *l1 = new TLegend(0.6,0.6,0.8,0.8);
l1->AddEntry(g1,"electron","l");
l1->AddEntry(g2,"#bar{#nu}","l");
l1->Draw();

//g1->Print();
}
