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

        virtual void PrintLine(std::string name);
        virtual void SetValidNames();
        virtual bool IsValid(std::string name);
        std::map<std::string, std::string> namemap;

    private:
        std::map<std::string, std::string> valids;
};