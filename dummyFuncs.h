#include <cmath>

#define dummyFunc_h

Double_t tofWcorr(double *x, double *p){
    Double_t xx = x[0];
    Double_t p0 = p[0];
    Double_t p1 = p[1];
    Double_t p2 = p[2];
    Double_t p3 = p[3];
    Double_t p4 = p[4];
    Double_t p5 = p[5];
    
    if (xx < p0) return p1 + (p2*exp(p3*xx));
    else return p1 + p4*xx + p5*xx*xx;
}

double g_peak(double *x, double *p){
    Double_t xx = x[0];
    Double_t p0 = p[0]; //t0
    Double_t p1 = p[1]; //A
    Double_t p2 = p[2]; //a0
    Double_t p3 = p[3]; //a1
    Double_t p4 = p[4]; //a2
    Double_t p5 = p[5]; //a3

    if (xx < p0) 
        return p1 * (p2 + 1) * pow(p3,2) / (pow(xx-p0,2) + pow(p3,2)) - p2;
    else if (xx >= p0)
        return p1 * (p4 + 1) * pow(p5,2) / (pow(xx-p0,2) + pow(p5,2)) - p4;
    else
        return -9999;
}

double hypBkgd(double *x, double *p){
    Double_t xx = x[0];
    Double_t p0 = p[0]; 
    Double_t p1 = p[1]; 
    Double_t p2 = p[2]; 
    Double_t p3 = p[3]; 
    Double_t p4 = p[4]; 

    return p0*cosh(p1*asinh(p2*(xx-p3)))+p4;
}

double gammaBkgd(double *x, double *p){
    Double_t xx = x[0];
    Double_t p0 = p[0]; 
    Double_t p1 = p[1]; 
    Double_t p2 = p[2]; 
    Double_t p3 = p[3]; 
    Double_t p4 = p[4];
    Double_t p5 = p[5];

    return p1*exp(-p2*(xx-p0))+p3*exp(-p4*(xx-p0))+p5;
}

double FullBkgd(double *x, double *p){
    Double_t xx = x[0];
    //Gamma Bkgd Params
    Double_t p0 = p[0]; 
    Double_t p1 = p[1]; 
    Double_t p2 = p[2]; 
    Double_t p3 = p[3]; 
    Double_t p4 = p[4];
    Double_t p5 = p[5];
    //Hyp Bkgd Params
    Double_t p6 = p[6]; 
    Double_t p7 = p[7]; 
    Double_t p8 = p[8]; 
    Double_t p9 = p[9]; 
    Double_t p10 = p[10]; 

    Double_t y;
    if(xx<2||xx>900) y = 0.;
    else y = p1*exp(-p2*(xx-p0))+p3*exp(-p4*(xx-p0))+p5 + p6*cosh(p7*asinh(p8*(xx-p9)))+p10;

    return y;
}

double RelHPGe_Eff(double *x, double *p){
    Double_t a0=1.46327e+09; 
    Double_t a1=-1.45652e+09; 
    Double_t a2=4.79578e+08; 
    Double_t a3=-5.51472e+07; 
    Double_t a4=333178; 
    Double_t a5=-1342.12;
    
    Double_t E = x[0];

    Double_t eff = (a0 + a1 * log(E) + a2 * pow(log(E),2) + a3 * pow(log(E),3) + a4 * pow(log(E),5) + a5 * pow(log(E),7))/E;

    return eff;    
}

double AbsHPGe_Eff(double *x, double *p){
    Double_t a0 = 14632.7;
    Double_t a1 = -14565.2;
    Double_t a2 = 4795.78;
    Double_t a3 = -551.472;
    Double_t a4 = 3.33177;
    Double_t a5 = -0.0134212;
    
    Double_t E = x[0];
    
    Double_t eff = (a0 + a1 * log(E) + a2 * pow(log(E),2) + a3 * pow(log(E),3) + a4 * pow(log(E),5) + a5 * pow(log(E),7))/E;

    return eff;
}

double AbsHPGe_Eff(double x){
    Double_t a0 = 14632.7;
    Double_t a1 = -14565.2;
    Double_t a2 = 4795.78;
    Double_t a3 = -551.472;
    Double_t a4 = 3.33177;
    Double_t a5 = -0.0134212;
    
    Double_t E = x;
    
    Double_t eff = (a0 + a1 * log(E) + a2 * pow(log(E),2) + a3 * pow(log(E),3) + a4 * pow(log(E),5) + a5 * pow(log(E),7))/E;

    return 0.75 * eff;
}