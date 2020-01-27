#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <string>

#define FileReader_hpp

using namespace std;


class FileReader {
    public:
        FileReader();
        virtual ~FileReader();
        //FileReader(char const *filename_);
        virtual bool Import();
        virtual bool SetFileName(char const *filename_=NULL){
            if (filename_==NULL) return false;
                filename = filename_;
                if (FILE *file = fopen(filename, "r")) {
                    fclose(file);
                    cout << "File " << filename << " exists.\n";
                    return true;
                } else {
                    cout << "File " << filename << " does not exist.\n";
                    return false;
                }
            };

        const char *filename;
        int nL;
        int nlines;
        std::string lineString;
};

//FileReader::FileReader(char const *filename_){
//    SetFileName(filename_);
//}

//bool FileReader::SetFileName(char const *filename_){
//}


