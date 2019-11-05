#include <cmath>
#include <iostream>

double nResponse(double *xx, double* p){
    double X = xx[0];
    double t0 = p[0];
    double a0 = p[1];
    double sig0 = p[2];
    double sig1 = p[3];
    double k1 = p[4];
    double d1 = p[5];
    double k2 = p[6];
    double d2 = p[7];
    double k3 = p[8];
    double d3 = p[9];
    double AA = p[10];

    //if (a0 < 0) a0 = 0;

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

    return AA*yy;
}  