#ifndef FileReader_hpp
#define FileReader_hpp

#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <string>

using namespace std;

class FileReader {
    public:
        FileReader();
        FileReader(const char *filename_);
        virtual ~FileReader();
        //FileReader(char const *filename_);
        virtual bool Import();
        virtual std::string GetLine();
        virtual bool OpenFile();
        virtual bool OpenFile(const char* filename_);
        virtual bool EoF(){ return infile.eof();};
        virtual bool SetFileName(char const *filename_=NULL){
            if (filename_==NULL) return false;
                filename = filename_;
                if (FILE *file = fopen(filename, "r")) {
                    fclose(file);
                    return true;
                } else {
                    cout << "File " << filename << " does not exist.\n";
                    return false;
                }
            };

        const char *filename;
        std::ifstream infile;
        int nL;
        int nlines;
        std::string lineString;
};


#endif