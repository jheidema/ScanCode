#include <stdio.h>

#include "HFReader.hpp"

int main(int argc, char *argv[]){

    HFReader tl;
    tl.SetFileName(argv[1]);
    tl.Import();
    tl.PrintInfo();
}