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
        vector<string> GetRow();
        vector<vector<string>> LoadFile();
        virtual bool OpenFile();
        virtual bool OpenFile(const char* filename_);
        virtual bool CloseFile();
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
        virtual bool IsOpen(){
            return infile.is_open();
        }
        void SetVerbose(bool kIn){kVerbose = kIn;};
        void Reset(){
            filename = "";
            nL = 0;
            nlines=0;
            lineString.clear();
            ftext.clear();
            CloseFile();
        }

        const char *filename;
        std::ifstream infile;
        int nL;
        int nlines;
        std::string lineString;
        vector<vector<string>> ftext;
        bool kVerbose = false;
};


#endif