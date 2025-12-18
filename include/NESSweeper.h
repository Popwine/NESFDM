#pragma once
#include "NESSolver.h"
#include <string>
class NESSweeper{
public:
    NESSweeper(NESSolver& solver_, std::string sweepParamsFile_, double totalMassRatio_);
    ~NESSweeper();
    void printDatas();
    void printConfigs();
    void run();
private:
    NESSolver& solver;
    int nesNum;
    std::string sweepParamsFile;
    double totalMassRatio;
    std::vector<std::vector<std::string>> lines;
    std::vector<std::vector<double>> mrDatas;
    std::vector<std::vector<double>> krDatas;
    std::vector<std::vector<double>> crDatas;

    void checkParamsIntegrity();
    void readParams();
    
    

};