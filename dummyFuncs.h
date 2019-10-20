#include <cmath>

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