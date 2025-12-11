#pragma once
#include <functional>
struct NES{
    double mr = 0.01;
    double kr = 1.0;
    double cr = 1.0;
    double m = 0.0;
    double k = 0.0;
    double c = 0.0;
};
class MainStructure{
public:
    MainStructure();
    void setFN(double fn_){fNatrual = fn_; refreshFReal(); refreshDynParams();};
    void setFNByMode(int mode);
    void setUStar(double u_){UStar = u_; refreshFReal(); };
    void print();
private:
	double rouFluid = 1.225;
    double UStar = 1.7;
    double mass = 7.32; // mass of the particle
	double fRealFactors[3] = { 0.968468691, 0.973652933, 0.979006853 };
	double dampingRatio = 0.003;
	double D = 0.0532;
	double B = 0.7117;
    double fNatrual = 1.117;
    // 被动参数
    double fReal = 1.0;
    double stiffness = 0.0;
    double damping = 0.0;

    void refreshFReal();
    void refreshDynParams();
    
public:
    double getFN() const{return fNatrual;};
};
class NESSolver{
public:
    NESSolver(const unsigned int nesNumber_);
    ~NESSolver();


private:
    const unsigned int nesNumber;
    const unsigned int dimension;
    MainStructure main;

    std::vector<NES> nes;
    double initialAStar = 0.06;
    double fDesign = 1.117;

    double taoStepSize = 0.001;
    double totalTao = 500;
    double resultCalcStartTao = 100.0;

    double timeStepSize = 0.001;
    double totalTime = 500;
    double resultCalcStartTime = 100.0;
public:
    void setUStar(double u_){main.setUStar(u_);};

    void setInitialAStar(double a_){initialAStar = a_; };
    void setFD(double fd_){fDesign = fd_; };
    void setFN(double fn_){main.setFN(fn_); };

    std::vector<std::function<void(double)>> setMr;
    std::vector<std::function<void(double)>> setKr;
    std::vector<std::function<void(double)>> setCr;
    std::vector<std::function<double(const std::vector<double>&)>> funcs;
public:
    double getFD() const{return fDesign;};
};