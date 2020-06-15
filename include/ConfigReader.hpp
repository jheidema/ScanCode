#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "FileReader.hpp"

#define ConfigReader_hpp

class ConfigReader: public FileReader {
    public:
        ConfigReader();
        virtual ~ConfigReader();
        bool Import() override;
        
        
        std::string GetHistogramName(){return namemap["hname"];};
        std::string GetLevelName(){return namemap["lname"];};
        std::string GetOutputName(){return namemap["output"];};
        std::string GetInputName(){return namemap["input"];};
        std::string GetBRFileName(){return namemap["brname"];};
        std::string GetFuncFileName(){return namemap["ffile"];};
             bool   GetGSFlag(){if(namemap.count("gsflag")==0) return false; else return stob[namemap["gsflag"]];};
             bool   GetGSFitFlag(){if(namemap.count("gsfit")==0) return false; else return stob[namemap["gsfit"]];};
             double GetGSScale(){
                    if(namemap.find("gscalc") != namemap.end()){
                        return std::stod(namemap.at("gscalc"));
                    }else{
                        std::cout << "No GSscale specified. Setting as 0.\n";
                        return 0.0;
                    }
                    };
        
        virtual void PrintLine(std::string name);
        virtual void SetValidNames();
        virtual bool IsValid(std::string name);
        std::map<std::string, std::string> namemap;

        void SetVerbose(bool kIn) {kVerbose = kIn;}

    private:
        std::map<std::string, bool> stob;
        std::map<std::string, std::string> valids;
        bool kVerbose;
};