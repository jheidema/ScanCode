#include <cmath>
#include <iostream>

#define calcResponse_hpp

double calcResponse(double *xx, double* p){
    double X = xx[0];
    double t0 = p[0];
    double AA = p[1];
    //double sigE = p[2];
    //double off = p[3];

    double a0 = 0.1667 + (-0.00141)*t0 + (5.27e-6)*t0*t0;
    double sig0 = 0.2594 + (0.000699)*t0 + 0.000202*t0*t0;
    double sig1 = -0.139 + 0.0242*t0;
    double k1 = 0.1819 - 0.000118*t0 + (8.215e-6)*t0*t0;
    double d1 = 75.41/(t0+17.70)+1.878;
    double k2 = 0.0162 - (4.797e-5)*t0 + (1.887e-6)*t0*t0;
    double d2 = 8.684 + 0.1331*t0 - (3.685e-4)*t0*t0;
    double k3 = 0.00335 + (2.108e-4)*t0 - (6.485e-7)*t0*t0;
    double d3 = 320.057 - 2.324*t0 + 0.00441*t0*t0;
    
        //hi-res
    /* double sigE = 0.7*(283600/2.*t0*t0-1663115*t0+681358889)*1.E-9;
    sig0 = 0.8*sqrt(sig0*sig0+0.65*sigE*sigE);
    sig1 = 1.*sqrt(sig1*sig1+1.*sigE*sigE); */
        //low-res
    double sigE = 0.9*(283600/2.*t0*t0-1663115*t0+681358889)*1.E-9;
    sig0 = 1.*sqrt(sig0*sig0+0.75*sigE*sigE);
    sig1 = 1.*sqrt(sig1*sig1+1.*sigE*sigE);

    k1=0.4*k1;
    d1=0.8*d1;
    k2=1.5*k2;
    d2=2.1*d2;

    double b1 = exp(k1*(k1+d1))/(1+pow(k1+d1,2));
    double b2 = exp(k1*(k1+d1)-(k1-k2)*(k2+d2))/(1+pow(k1+d1,2));
    double b3 = exp(k1*(k1+d1)-(k1-k2)*(k2+d2)-(k2-k3)*(k3+d3))/(1+pow(k1+d1,2));

    double yy=0;
    double x = X-t0;

    double A = -sig0*sqrt(a0)+(1+a0)*sig0*atan(1/sqrt(a0))
                + sig1*atan(k1+d1)
                + sig1*(1-exp(k1*(d1-d2+k1-k2)))/(k1*(1+d1*d1+2*d1*k1+k1*k1))
                + sig1*exp(k1*(d1-d2+k1-k2)-k2*(d3+k3))/(k2*(1+d1*d1+2*d1*k1+k1*k1))*(exp(k2*(d3+k3))-exp(k2*(d2+k2)))
                + sig1*exp(k1*(d1-d2+k1-k2)+k2*(d2-d3+k2-k3))/(k3*(1+d1*d1+2*d1*k1+k1*k1));

    if (x >= -1/sqrt(a0)*sig0 && x <= 0)
        yy=sig0*sig0/((x*x+sig0*sig0))*(a0+1)-a0;

    else if (x > 0 && x <= (k1+d1)*sig1)
        yy=sig1*sig1/(x*x+sig1*sig1);

    else if (x > (k1+d1)*sig1 && x <= (k2+d2)*sig1)
        yy=b1*exp(-k1*x/sig1);

    else if (x > (k2+d2)*sig1 && x <= (k3+d3)*sig1)
        yy=b2*exp(-k2*x/sig1);
        //yy=0;
    else if (x > (k3+d3)*sig1)
        yy=b3*exp(-k3*x/sig1);
        //yy=0;

    return AA*yy ;
}  