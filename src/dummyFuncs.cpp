#include "dummyFuncs.hpp"
#ifdef dummyFunc_hpp

#include <cmath>
#include "TF1.h"

double tofWcorr(double *x, double *p){
    double xx = x[0];
    double p0 = p[0];
    double p1 = p[1];
    double p2 = p[2];
    double p3 = p[3];
    double p4 = p[4];
    double p5 = p[5];
    
    if (xx < p0) return p1 + (p2*exp(p3*xx));
    else return p1 + p4*xx + p5*xx*xx;
}

double g_peak(double *x, double *p){
    double xx = x[0];
    double p0 = p[0]; //t0
    double p1 = p[1]; //A
    double p2 = p[2]; //a0
    double p3 = p[3]; //a1
    double p4 = p[4]; //a2
    double p5 = p[5]; //a3

    if (xx < p0) 
        return p1 * (p2 + 1) * pow(p3,2) / (pow(xx-p0,2) + pow(p3,2)) - p2;
    else if (xx >= p0)
        return p1 * (p4 + 1) * pow(p5,2) / (pow(xx-p0,2) + pow(p5,2)) - p4;
    else
        return -9999;
}

double hypBkgd(double *x, double *p){
    double xx = x[0];
    double p0 = p[0]; 
    double p1 = p[1]; 
    double p2 = p[2]; 
    double p3 = p[3]; 
    double p4 = p[4]; 

    return p0*cosh(p1*asinh(p2*(xx-p3)))+p4;
}

double pieceBkgd(double *x, double *p){
    double xx = x[0];
    double p0 = p[0]; 
    double p1 = p[1]; 
    double p2 = p[2]; 
    double p3 = p[3]; 
    
    double res = 0.;
    if(xx <= p0) res = p1*xx+p2;
    if(xx > p0) res = p3*xx+(p1-p3)*p0+p2;

    if (res > 0.) return res;
    else return 0;
}

double gammaBkgd(double *x, double *p){
    double xx = x[0];
    double p0 = p[0]; 
    double p1 = p[1]; 
    double p2 = p[2]; 
    double p3 = p[3]; 
    double p4 = p[4];
    double p5 = p[5];
    double p6 = p[6];

    if(xx>25&&xx<200) TF1::RejectPoint();
    return p1*exp(-p2*(xx-p0))+p3*exp(-p4*(xx-p6))+p5;
    //return p1*exp(-p2*(xx-p0))+p3*exp(-p4*(xx-p0))+p5;
}

double FullBkgd(double *x, double *p){
    double xx = x[0];
    //Gamma Bkgd Params
    //double p0 = p[0]; 
    //double p1 = p[1]; 
    //double p2 = p[2]; 
    //double p3 = p[3]; 
    //double p4 = p[4];
    //double p5 = p[5];
    ////Hyp Bkgd Params
    //double p6 = p[6]; 
    //double p7 = p[7]; 
    //double p8 = p[8]; 
    //double p9 = p[9]; 
    //double p10 = p[10]; 

    double y;
    if(xx<2||xx>900) { y = 0.; /* TF1::RejectPoint(); */}
    else y = gammaBkgd(x,p) + pieceBkgd(x,&p[7]);
    //else y = gammaBkgd(x,p) + hypBkgd(x,&p[6]);
    //else y = p1*exp(-p2*(xx-p0))+p3*exp(-p4*(xx-p0))+p5 + p6*cosh(p7*asinh(p8*(xx-p9)))+p10;

    //if(xx>30&&xx<200) TF1::RejectPoint();

    return y;
}

double MadeyQ(double *x, double *p){
    double xx = x[0];

    double p0 = p[0];
    double p1 = p[1];
    double p2 = p[2];
    double p3 = p[3];

    return p0*xx-p1*(1.0-exp(-1.0*p2*pow(xx,p3)));
}
double MadeyQ(double x, double *p){
    double xx = x;

    double p0 = p[0];
    double p1 = p[1];
    double p2 = p[2];
    double p3 = p[3];

    return p0*xx-p1*(1.0-exp(-1.0*p2*pow(xx,p3)));
}
double QDCEdge(double *x, double *p){
    double xx =  0.5 * 939.57 * pow(100./x[0]/29.98,2);
    
    double p0 = p[0];
    double p1 = p[1];

    double MadQ = MadeyQ(xx,&p[2]);
    return p0 + p1 * (MadQ*1000.);
}


double RelHPGe_Eff(double *x, double *p){
    double a0=1.46327e+09; 
    double a1=-1.45652e+09; 
    double a2=4.79578e+08; 
    double a3=-5.51472e+07; 
    double a4=333178; 
    double a5=-1342.12;
    
    double E = x[0];

    double eff = (a0 + a1 * log(E) + a2 * pow(log(E),2) + a3 * pow(log(E),3) + a4 * pow(log(E),5) + a5 * pow(log(E),7))/E;

    return eff;    
}

double AbsHPGe_Eff(double *x, double *p){
    double a0 = 14632.7;
    double a1 = -14565.2;
    double a2 = 4795.78;
    double a3 = -551.472;
    double a4 = 3.33177;
    double a5 = -0.0134212;
    
    double E = x[0];
    
    double eff = (a0 + a1 * log(E) + a2 * pow(log(E),2) + a3 * pow(log(E),3) + a4 * pow(log(E),5) + a5 * pow(log(E),7))/E;

    return eff;
}

double AbsHPGe_Eff(double x){
    double a0 = 14632.7;
    double a1 = -14565.2;
    double a2 = 4795.78;
    double a3 = -551.472;
    double a4 = 3.33177;
    double a5 = -0.0134212;
    
    double E = x;
    
    double eff = (a0 + a1 * log(E) + a2 * pow(log(E),2) + a3 * pow(log(E),3) + a4 * pow(log(E),5) + a5 * pow(log(E),7))/E;

    return 0.75 * eff;
}

#endif