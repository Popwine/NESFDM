#include "NESSweeper.h"
#include <algorithm>
#include <fstream>

NESSweeper::NESSweeper(NESSolver& solver_, std::string sweepParamsFile_, double totalMassRatio_)
:solver(solver_), 
nesNum(solver_.getNESNumber()),
sweepParamsFile(sweepParamsFile_),
totalMassRatio(totalMassRatio_)
{
    if(nesNum < 1){
        throw std::runtime_error("NES number must be greater than 0 when sweeping.");
    }
    std::ifstream ifs(sweepParamsFile);
    std::string line;

    
    while(std::getline(ifs, line)){
        std::istringstream iss(line);
        std::vector<std::string> wordsInLine;
        std::string word;
        while(iss >> word){
            wordsInLine.push_back(word);
        }

        if(wordsInLine.empty()){
            continue;
        }

        if(wordsInLine.size() > 1){
            lines.push_back(wordsInLine);
        }
        
    }

    checkParamsIntegrity();
    readParams();
    //printConfigs();
    
    // for(const auto& wordInLine : words){
    //     for(const auto& word : wordInLine){
    //         std::cout << "\"" << word << "\"" ;
    //     }
    //     std::cout << std::endl;
    // }
    ifs.close();

}
NESSweeper::~NESSweeper(){

}
void NESSweeper::printDatas(){
    for(size_t i = 0; i < nesNum; i++){
        std::cout << "NES " + std::to_string(i + 1) + " Sweep Params: " << std::endl;
        if(i == nesNum - 1){
            std::cout << "mr:\t defined by others" << std::endl;
        }
        else{
            std::cout << "mr:\t";
            for(const double data : mrDatas.at(i)){
                std::cout << data << "\t";
            }
            std::cout << std::endl;
        }
        


        std::cout << "kr:\t";
        for(const double data : krDatas.at(i)){
            std::cout << data << "\t";
        }
        std::cout << std::endl;

        std::cout << "cr:\t";
        for(const double data : crDatas.at(i)){
            std::cout << data << "\t";
        }
        std::cout << std::endl;
    }
}
void NESSweeper::printConfigs(){
    // 用于存储当前递归路径中选中的参数
    std::vector<double> currentMr;
    std::vector<double> currentKr;
    std::vector<double> currentCr;
    
    // 计数器，用于标记是第几种组合
    size_t configIndex = 0;

    // ---------------------------------------------------------
    // 定义递归 Lambda 函数
    // 注意：为了让 lambda 能递归调用自身，必须显式指明类型 std::function
    // ---------------------------------------------------------

    // Level 3: 遍历 Damping (cr)
    std::function<void(size_t)> sweepCr = [&](size_t idx) {
        // 如果所有 NES 的 cr 都选好了
        if(idx == nesNum) {
            configIndex++;
            std::cout << "Config " << configIndex << ":\t" ;
            
            // 打印 Mass Ratios
           
            for(const auto& val : currentMr) std::cout << val << "\t";
            

            // 打印 Stiffness Ratios
            
            for(const auto& val : currentKr) std::cout << val << "\t";
            

            // 打印 Damping Ratios
            
            for(const auto& val : currentCr) std::cout << val << "\t";
           
            std::cout << std::endl;
            
            return;
        }
        
        // 递归遍历当前层级 (NES idx) 的所有可能 cr 值
        for(const double& val : crDatas[idx]) {
            currentCr.push_back(val);
            sweepCr(idx + 1);
            currentCr.pop_back(); // 回溯
        }
    };

    // Level 2: 遍历 Stiffness (kr)
    std::function<void(size_t)> sweepKr = [&](size_t idx) {
        // 如果所有 NES 的 kr 都选好了，进入下一阶段：选 cr
        if(idx == nesNum) {
            sweepCr(0);
            return;
        }

        // 递归遍历当前层级 (NES idx) 的所有可能 kr 值
        for(const double& val : krDatas[idx]) {
            currentKr.push_back(val);
            sweepKr(idx + 1);
            currentKr.pop_back(); // 回溯
        }
    };

    // Level 1: 遍历 Mass (mr)
    // idx: 当前正在决定的 NES 索引
    // currentSum: 当前已确定的质量比之和
    std::function<void(size_t, double)> sweepMr = [&](size_t idx, double currentSum) {
        // 基本情况：前 nesNum - 1 个质量比已经确定
        if(idx == nesNum - 1) {
            // 计算最后一个质量比
            double lastMr = totalMassRatio - currentSum;
            
            // 检查物理有效性：质量必须大于 0 (使用一个小容差防止浮点误差)
            if(lastMr > 1e-9) {
                currentMr.push_back(lastMr);
                
                // 质量组合确定且有效，开始遍历 Stiffness
                sweepKr(0);
                
                currentMr.pop_back(); // 回溯
            }
            // 如果 lastMr <= 0，则该路径无效，直接返回（剪枝）
            return;
        }

        // 递归遍历 mrDatas 中的候选项
        for(const double& val : mrDatas[idx]) {
            // 优化：如果加上当前值已经超过总质量比，后续必定无效，可提前剪枝
            if(currentSum + val < totalMassRatio) {
                currentMr.push_back(val);
                sweepMr(idx + 1, currentSum + val);
                currentMr.pop_back(); // 回溯
            }
        }
    };

    // ---------------------------------------------------------
    // 开始执行
    // ---------------------------------------------------------
    
    // 从第 0 个 NES 的质量开始递归
    sweepMr(0, 0.0);

    std::cout << "Total configurations generated: " << configIndex << std::endl;
}

void NESSweeper::run(){
    if(nesNum == 1){
        solver.setNESMr(1, totalMassRatio);
        for(const double kr : krDatas[0])
        for(const double cr : crDatas[0]){
            std::cout << totalMassRatio << "\t" << kr << "\t" << cr << std::endl;
            
        }
    }
}
void NESSweeper::checkParamsIntegrity(){
    std::vector<bool> mrFlag(nesNum - 1, false);
    std::vector<bool> krFlag(nesNum, false);
    std::vector<bool> crFlag(nesNum, false);

    for(const auto& l : lines){
        auto firstWord = l.at(0);
        if(firstWord.size() >= 3 && firstWord.compare(0, 2, "mr") == 0){
            std::string strIndex = firstWord.substr(2, firstWord.size() - 1);
            size_t index;
            try{
                index = std::stoi(strIndex);
            }
            catch (const std::exception& e){
                throw std::runtime_error("In sweep parameters file: " + sweepParamsFile + ": wrong format of index: " + firstWord + "\n waht(): " + e.what() );
            }
            //std::cout << "index: " << index << std::endl;

            if(index == 0 || index >= nesNum){
                throw std::runtime_error("In sweep parameters file: " + sweepParamsFile + ": index of " + firstWord + " is out of range (0~" + std::to_string(nesNum - 1) + ")" 
                + ((index == nesNum) ? (", mr" + std::to_string(nesNum) + " should be specified by --total-mass-ratio and first " + std::to_string(nesNum-1) + " mass ratios."  ) : ""));
            }
            if(mrFlag[index - 1] == false){
                mrFlag[index - 1] = true;
            }
            else{
                throw std::runtime_error("Repeated defination of mr" + std::to_string(index) );
            }
            
        }
        if(firstWord.size() >= 3 && firstWord.compare(0, 2, "kr") == 0){
            std::string strIndex = firstWord.substr(2, firstWord.size() - 1);
            size_t index;
            try{
                index = std::stoi(strIndex);
            }
            catch (const std::exception& e){
                throw std::runtime_error("In sweep parameters file: " + sweepParamsFile + ": wrong format of index: " + firstWord + "\n waht(): " + e.what() );
            }
            //std::cout << "index: " << index << std::endl;

            if(index == 0 || index > nesNum){
                throw std::runtime_error("In sweep parameters file: " + sweepParamsFile + ": index of " + firstWord + " is out of range (0~" + std::to_string(nesNum - 1) + ")" );
            }
            if(krFlag[index - 1] == false){
                krFlag[index - 1] = true;
            }
            else{
                throw std::runtime_error("Repeated defination of kr" + std::to_string(index) );
            }
        }
        if(firstWord.size() >= 3 && firstWord.compare(0, 2, "cr") == 0){
            std::string strIndex = firstWord.substr(2, firstWord.size() - 1);
            size_t index;
            try{
                index = std::stoi(strIndex);
            }
            catch (const std::exception& e){
                throw std::runtime_error("In sweep parameters file: " + sweepParamsFile + ": wrong format of index: " + firstWord + "\n waht(): " + e.what() );
            }
            //std::cout << "index: " << index << std::endl;

            if(crFlag[index - 1] == false){
                crFlag[index - 1] = true;
            }
            else{
                throw std::runtime_error("Repeated defination of cr" + std::to_string(index) );
            }
        }
        
    }
    for(const auto flag : mrFlag){
        if(!flag){ throw std::runtime_error("Sweep parameters mr is not complete.");}
    }
    for(const auto flag : krFlag){
        if(!flag){ throw std::runtime_error("Sweep parameters kr is not complete.");}
    }
    for(const auto flag : crFlag){
        if(!flag){ throw std::runtime_error("Sweep parameters cr is not complete.");}
    }
}

void NESSweeper::readParams(){
    mrDatas.resize(nesNum - 1);
    krDatas.resize(nesNum);
    crDatas.resize(nesNum);
    for(const auto& l : lines){
        auto firstWord = l.at(0);
        size_t wordNum = l.size();
        std::vector<double> datas;
        for(size_t i = 1; i < wordNum; i++){
            try{
                datas.push_back(std::stod(l[i]));
            }catch(const std::exception& e){
                throw std::runtime_error("Data of " + firstWord + " can't be converted.");
            }
        }
        if(datas.size() == 3 && (datas[1] > datas[0] && datas[1] > datas[2])){
            // 给出起点终点和步长的情况
            double start = datas[0];
            double end = datas[1]; 
            double step = datas[2];
            //std::cout << "\"" << datas[2] << "\"" << std::endl;
            datas.clear();
            for(double data = start; data < end + step * 1e-8; data += step){
                datas.push_back(data);
            }
        }
        std::string strIndex = firstWord.substr(2, firstWord.size() - 1);
        size_t index;
        index = std::stod(strIndex);
        for(size_t i = 0; i < datas.size() - 1; i++){
            if(datas[i] > datas[i+1]){
                throw std::runtime_error(
                    "Wrong parameter oder of " + firstWord +": Data must be in positive sequence."
                );
            }
        }
        if(firstWord.size() >= 3 && firstWord.compare(0, 2, "mr") == 0){
            mrDatas[index - 1] = datas;
            
        }
        else if(firstWord.size() >= 3 && firstWord.compare(0, 2, "kr") == 0){
            krDatas[index - 1] = datas;
        }
        else if(firstWord.size() >= 3 && firstWord.compare(0, 2, "cr") == 0){
            crDatas[index - 1] = datas;
        }
        
    }
    
}
