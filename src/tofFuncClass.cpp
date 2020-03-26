#include "tofFuncClass.hpp"

#ifdef TofFuncClass_hpp

tofFuncClass::tofFuncClass(){
}

tofFuncClass::~tofFuncClass(){
    Clear();
}

void tofFuncClass::InsertFunction(double t0, double amp){
    nFuncs++;
    params.push_back(t0);
    params.push_back(amp);
}

void tofFuncClass::RemoveFunction(int iF){
    
    if(iF>nFuncs){cout << "Function/Peak " << iF << " does not exist..." << endl; return;}
    if(iF<=0) {cout<<"Function number must be between 1 and nFuncs." << endl; return;}
    cout << "Removing Function " << iF << endl;
    params.erase(params.begin()+(iF-1)*2,params.begin()+iF*2);
    nFuncs--;
    return;
}

void tofFuncClass::SortParams(){
    if(nFuncs<2) return;

    double pvals[2];
    vector<double> copy_vec;
    copy_vec.push_back(params.at(0));
    copy_vec.push_back(params.at(1));
    std::vector<double>::iterator it;

    for(int ip=2; ip<nFuncs*npars; ip=ip+2){
        pvals[0]=params.at(ip);
        pvals[1]=params.at(ip+1);
        for(it = copy_vec.begin(); it<copy_vec.end(); it++){
            if(params.at(ip)<*it)  break;
        }
        copy_vec.insert(it,pvals,pvals+2);   
    }
    params.clear();
    params = copy_vec;
}

void tofFuncClass::SetParams(double *p){
    for(int ip=0; ip<nFuncs*npars; ip++){
        params.at(ip) = p[ip];
    }
}

void tofFuncClass::SetParams(int iF, double *p){
    for(int ip=0; ip<npars; ip++){
        params.at(ip+(iF*npars)) = p[ip];
    }
}

void tofFuncClass::SetParam(int iF, int iP, double p){
    params.at(iP+iF*npars) = p;
}

void tofFuncClass::Clear(){
    nFuncs = 0;
    params.clear();
    for (int ip=0; ip<npars; ip++) pArray[ip] = 0.0;
}

#endif