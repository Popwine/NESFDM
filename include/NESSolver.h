#pragma once
#include <functional>
#include "ModelParameters.h"
#include "NESFDMUtils.h"
struct NES{
    double mr = 0.01;
    double kr = 1.0;
    double cr = 1.0;
    double m = 0.0;
    double k = 0.0;
    double c = 0.0;
    double invM = 0.0;
};
class MainStructure{
public:
    MainStructure(double u = 1.7, double fn = 1.117);
    void setFN(double fn_){fNatrual = fn_; refreshFReal(); refreshDynParams();};
    void setFNByMode(int mode);
    void setUStar(double u_){UStar = u_; refreshFReal(); };
    void setDampingRatio(double ksi_){dampingRatio = ksi_; refreshDynParams();}
    void print() const;
private:
	const double rouFluid = 1.225;
    double UStar = 1.7;
    const double mass = 7.32; // mass of the particle
	const double fRealFactors[3] = { 0.968468691, 0.973652933, 0.979006853 };
	double dampingRatio = 0.003;
	const double D = 0.0532;
	const double B = 0.7117;
    double fNatrual = 1.117;
    // 被动参数

    double fReal = 1.0;
    double stiffness = 0.0;
    double damping = 0.0;

    void refreshFReal();
    void refreshDynParams();


public:
    double getRou() const{return rouFluid;};
    double getUstar() const{return UStar;};
    double getM() const{return mass;};
    double getKsi() const{return dampingRatio;};
    double getD() const{return D;};
    double getB() const{return B;};
    double getFN() const{return fNatrual;};
    double getFR() const{return fReal;};
    double getK() const{return stiffness;};
    double getC() const{return damping;};


};
class NESSolver{
public:
    NESSolver(const unsigned int nesNumber_);
    ~NESSolver();


private:
    const unsigned int nesNumber;
    const unsigned int dimension;
    MainStructure main;
    ModelParameters model;


    std::vector<NES> nes;
    
    double initialAStar = 0.06;
    double fDesign = 1.117;
    double ksiDesign = 0.003;
    // tao = f * time
    double taoStepSize = 0.001;
    double totalTao = 500;
    double resultCalcStartTao = 250.0;

    double timeStepSize = 0.001;
    double totalTime = 500;
    double resultCalcStartTime = 250.0;

    double kDesign = 0.0;
    double cDesign = 0.0;

    std::string outputFile = "";
public:
    

    void setInitialAStar(double a_){initialAStar = a_; };
    void setFD(double fd_);
    void setMainFN(double fn_);
    void setMainDampingRatio(double ksi_);
    void setDesignDampingRatio(double ksiDesign_);
    void setUStar(double u_);
    void setMainFNByMode(int mode_);
    void setTaoStepSize(double taoStepSize_);
    void setTotalTao(double totalTao_);
    void setResultCalcStartTao(double resultCalcStartTime_);
    void setOutput(std::string outputFile_){outputFile = outputFile_;};

    void setNESMr(size_t i, double mr_);
    void setNESKr(size_t i, double kr_);
    void setNESCr(size_t i, double cr_);
    void printMain() const{main.print();};

    DisplacementResults run();
    std::vector<DisplacementResults> runConfig3m3u();
    std::vector<DisplacementResults> runConfig1m3u();
public:
    std::vector<std::function<double(const std::vector<double>&)>> funcs;
public:
    double getFD() const{return fDesign;};
    int getNESNumber() const{return nesNumber;};
    void refreshAll();
    void printAll() const;
private:
    void refreshDesignValue();
    void refreshTao();
    void refreshFuncs();
    void refreshNES();
    void refreshModelParameters();

};
