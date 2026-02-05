#include "NESSolver.h"

#include "RungeKutta4.h"
#include <math.h>

NESSolver::NESSolver(const unsigned int nesNumber_):
nesNumber(nesNumber_),
dimension(3 + 2 * nesNumber_),
main(),
model(main.getUstar())
{
    nes.resize(nesNumber);



    
    refreshTao();
    refreshDesignValue();
    refreshNES();
    refreshFuncs();
    

}

NESSolver::~NESSolver(){
    
}
void NESSolver::setFD(double fd_){
    fDesign = fd_; 
    refreshAll();
};
void NESSolver::setMainFN(double fn_){
    main.setFN(fn_); 
    refreshAll();
};
void NESSolver::setMainDampingRatio(double ksi_){
    main.setDampingRatio(ksi_); 
    refreshAll();
}
void NESSolver::setDesignDampingRatio(double ksiDesign_){
    ksiDesign = ksiDesign_;
    refreshAll();
}
void NESSolver::setUStar(double u_){
    main.setUStar(u_); 
    refreshDesignValue();
    refreshNES();
    refreshFuncs();
};
void NESSolver::setMainFNByMode(int mode_){
    main.setFNByMode(mode_);
    refreshDesignValue();
    refreshNES();
    refreshFuncs();
};
void NESSolver::setTaoStepSize(double taoStepSize_){
    taoStepSize = taoStepSize_;
    refreshTao();
}
void NESSolver::setTotalTao(double totalTao_){
    totalTao = totalTao_;
    refreshTao();
}
void NESSolver::setResultCalcStartTao(double resultCalcStartTime_){
    resultCalcStartTao = resultCalcStartTime_;
    refreshTao();
}

void NESSolver::setNESMr(size_t i, double mr_){
    if(i == 0 || i > nesNumber){
        throw std::runtime_error("Index out of range in setNESMr, i is a 1-based index.");
    }
    if(mr_ < 0){
        throw std::runtime_error("Mass ratio of NES must be positive.");
    
    }
    this->nes[i-1].mr = mr_;
    this->nes[i-1].m = main.getM() * mr_;
}
void NESSolver::setNESKr(size_t i, double kr_){
    if(i == 0 || i > nesNumber){
        throw std::runtime_error("Index out of range in setNESKr, i is a 1-based index.");
    }
    if(kr_ < 0){
        throw std::runtime_error("Stiffness ratio of NES must be positive.");
    
    }
    refreshDesignValue();
    this->nes[i-1].kr = kr_;
    this->nes[i-1].k = kDesign * kr_ / main.getD() / main.getD();
}
void NESSolver::setNESCr(size_t i, double cr_){
    if(i == 0 || i > nesNumber){
        throw std::runtime_error("Index out of range in setNESCr, i is a 1-based index.");
    }
    if(cr_ < 0){
        throw std::runtime_error("Damping ratio of NES must be positive.");
    
    }
    refreshDesignValue();
    this->nes[i-1].cr = cr_;
    this->nes[i-1].c = cDesign * cr_;
}
DisplacementResults NESSolver::run(){
    refreshAll();
    int numSteps = static_cast<int>(totalTime / timeStepSize);
    
    RungeKutta4 rk4(dimension, timeStepSize, numSteps, funcs);
    double D = main.getD();
    std::vector<double> state;
    state.push_back(0.0);
    for(int i = 1; i <= nesNumber + 1; i++){
        state.push_back(initialAStar * D);
    }
    for(int i = 1; i <= nesNumber + 1; i++){
        state.push_back(0.0);
    }
    
    
    std::ofstream ofs(outputFile);
	std::vector<std::vector<double>> results;
	std::function<void(const std::vector<double>&)> stepFunction;
	if (!outputFile.empty()) {
        
		stepFunction =
			[&ofs, &results](const std::vector<double>& state) {
			for (const auto& val : state) {
				ofs << std::scientific << std::setprecision(10) << val << "\t";
			}
			ofs << "\n";
			results.push_back(state);
			};
	}
	else {
		stepFunction =
			[&results](const std::vector<double>& state) {
			results.push_back(state);
			};
	}

	rk4.setStepFunction(stepFunction);
	rk4.integrate(state);
    ofs.close();
    
	double yRms = getRms(results, 1, resultCalcStartTime) / main.getD();
	double yMax = getMax(results, 1, resultCalcStartTime) / main.getD();
	return DisplacementResults{ yRms,yMax };

}

std::vector<DisplacementResults> NESSolver::runConfig3m3u(){
    double U_stars[3] = { 1.6, 1.7, 1.8 };
    double naturalFreq[3] = { 0.1705 / 0.2325 * 1.117, 1.117, 0.3687 / 0.2325 * 1.117 };
    std::vector<DisplacementResults> allResults;
    for (double fn : naturalFreq) {
        setMainFN(fn);
        for (double U_star : U_stars) {
            setUStar(U_star);
            auto results = run();
            allResults.push_back(results);
            
        }
        
    }
    if(allResults.size() != 9){
        throw std::runtime_error("Number of results for 3m3u is not 9!");
    }
    return allResults;
}
std::vector<DisplacementResults> NESSolver::runConfig1m3u(){
    double U_stars[3] = { 1.6, 1.7, 1.8 };
    std::vector<DisplacementResults> allResults;

    for (double U_star : U_stars) {
        setUStar(U_star);
        auto results = run();
        allResults.push_back(results);
        
    }
        
    
    if(allResults.size() != 3){
        throw std::runtime_error("Number of results for 1m3u is not 3!");
    }
    return allResults;
}
void NESSolver::refreshDesignValue(){
    kDesign = main.getM() * (2 * PI * 1.0 * fDesign) * (2 * PI * 1.0 * fDesign);

	cDesign = 2 * ksiDesign * sqrt(kDesign * main.getM());
}
void NESSolver::refreshTao(){
    timeStepSize = taoStepSize / main.getFN();
    totalTime = totalTao / main.getFN();
    resultCalcStartTime = resultCalcStartTao / main.getFN();
}
void NESSolver::refreshFuncs(){

    refreshDesignValue();
    funcs.clear();
    double omega = 2 * PI * main.getFR();
    double rou = main.getRou();
    double B = main.getB();
    double D = main.getD();
    double ypDotFactor = PI * rou * B * main.getFR() * B;
    double ypFactor = 2 * PI * PI * rou * B * B * B * main.getFR() * main.getFR() / D;
    // function 0 t
    funcs.push_back([](const std::vector<double>& state) { return 1; }); // t
    funcs.push_back([this](const std::vector<double>& state) { 
        return state[nesNumber + 2];
    }); // yp_dot
    for(int i = 1; i <= nesNumber; i++){
        funcs.push_back([this, i](const std::vector<double>& state) 
        { 
            return state[nesNumber + 2 + i];
        }); // yai_dot
    }
    double invMainM = 1.0 / main.getM();
    double invMainD = 1.0 / main.getD();
    double invOmega = 1.0 / omega;
    funcs.push_back([this, invMainM, invMainD, invOmega, ypDotFactor, ypFactor](const std::vector<double>& state){
        double fl;
        double yp = state[1];
        double ypv = state[nesNumber + 2];
        
        double current_A_star = std::sqrt(yp * yp + ypv * ypv * invOmega * invOmega) * invMainD;
        double h1, h4;
        //std::vector<double> ya(nesNumber), yav(nesNumber);

        model.getAeroCoeffs(current_A_star, h1, h4);
        fl = ypDotFactor * h1 * ypv + ypFactor * h4 * yp;


        double primaryDampingTerm = -main.getC() * state[nesNumber + 2]; // -damping * yp_dot
        double primaryStiffnessTerm = -main.getK() * state[1]; // -stiffness * yp
        double nesDampingTerm = 0.0, nesStiffnessTerm = 0.0;
        for(size_t i = 1; i <= nesNumber; i++){
            nesDampingTerm += -nes[i-1].c * (ypv - state[i + nesNumber + 2]);
        }
        for(size_t i = 1; i <= nesNumber; i++){
            nesStiffnessTerm += 
                -nes[i-1].k * (yp - state[i + 1])
                            * (yp - state[i + 1])
                            * (yp - state[i + 1]);
            
        }
        return (
            fl 
            + primaryDampingTerm 
            + primaryStiffnessTerm 
            + nesDampingTerm 
            + nesStiffnessTerm
        ) * invMainM;
    });// yp_dot_dot
    for(int i = 1; i <= nesNumber; i++){
        
        funcs.push_back([this, i](const std::vector<double>& state) 
        { 
            double yp = state[1];
            double ypv = state[nesNumber + 2];
            return (
                -nes[i-1].c * (state[i + nesNumber + 2] - ypv)
                -nes[i-1].k * (state[i + 1] - yp) * (state[i + 1] - yp) * (state[i + 1] - yp)
            ) * nes[i-1].invM;
        }); // yai_dot_dot
    }
}
void NESSolver::refreshNES(){
    for(auto& n : nes){
        n.m = main.getM() * n.mr;
        n.k = kDesign * n.kr / main.getD() / main.getD();
        n.c = cDesign * n.cr;
    }
    for(auto& n : nes){ // 注意加引用 &，否则修改不到原数组
        n.m = main.getM() * n.mr;
        n.invM = 1.0 / n.m; // 预计算倒数

    }

}
void NESSolver::refreshModelParameters(){
    model = ModelParameters(main.getUstar());
}
void NESSolver::refreshAll(){
    refreshTao();
    refreshDesignValue();
    refreshNES();
    refreshFuncs();
    refreshModelParameters();
}
void NESSolver::printAll() const{
    
    std::cout << "-----------------Solver parameters-----------------" << std::endl;
    std::cout << "nesNumber: " << nesNumber << std::endl;
    std::cout << "dimension: " << dimension << std::endl;
    std::cout << "initialAStar: " << initialAStar << std::endl;
    std::cout << "fDesign: " << fDesign << std::endl;
    std::cout << "taoStepSize: " << taoStepSize << std::endl;
    std::cout << "totalTao: " << totalTao << std::endl;
    std::cout << "resultCalcStartTao: " << resultCalcStartTao << std::endl;
    std::cout << "timeStepSize: " << timeStepSize << std::endl;
    std::cout << "totalTime: " << totalTime << std::endl;
    std::cout << "resultCalcStartTime: " << resultCalcStartTime << std::endl;
    std::cout << "ksiDesign: " << ksiDesign << std::endl;
    std::cout << "kDesign: " << kDesign << std::endl;
    std::cout << "cDesign: " << cDesign << std::endl;
    std::cout << "outputFile: " << outputFile << std::endl;
    int i = 1;
    std::cout << "-------------------NES parameters------------------" << std::endl;
    for(auto n : nes){
        std::cout << "ma_" << i << ": " << n.m << std::endl;
        std::cout << "ka_" << i << ": " << n.k << std::endl;
        std::cout << "ca_" << i << ": " << n.c << std::endl;
        i++;
    }

    main.print();

}
MainStructure::MainStructure(double u, double fn):
UStar(u),
fNatrual(fn)
{
    refreshFReal();
    refreshDynParams();
}
void MainStructure::setFNByMode(int mode){
    if(mode == 1){
        fNatrual = 0.1705 / 0.2325 * 1.117;
    }
    else if(mode == 2){
        fNatrual = 1.117;
    }
    else if(mode == 3){
        fNatrual = 0.3687 / 0.2325 * 1.117;
    }
    else{
        throw std::runtime_error("Unsupported mode in function \"MainStructure::setFNByMode(int mode)\"");
    }
    refreshFReal();
    refreshDynParams();
}
void MainStructure::refreshFReal(){
    if(isEQ(UStar, 1.6)){
        fReal = fNatrual * fRealFactors[0];
    }
    else if(isEQ(UStar, 1.7)){
        fReal = fNatrual * fRealFactors[1];
    }
    else if(isEQ(UStar, 1.8)){
        fReal = fNatrual * fRealFactors[2];
    }else{
        throw std::runtime_error("Unsupported U*.");
    }
    
}

void MainStructure::refreshDynParams(){
    stiffness = mass * (2 * PI * 1.0 * fNatrual) * (2 * PI * 1.0 * fNatrual);

	damping = 2 * dampingRatio * sqrt(stiffness * mass);
}
void MainStructure::print() const{
    std::cout << "-------------Main structure parameters-------------" << std::endl;
    std::cout << "Fluid density: " << rouFluid << std::endl;
    std::cout << "Reduced wind velocity: " << UStar << std::endl;
    std::cout << "Mass: " << mass << std::endl;
    std::cout << "Damping Ratio: " << dampingRatio << std::endl;
    std::cout << "Height: " << D << std::endl;
    std::cout << "Width: " << B << std::endl;
    std::cout << "Natural frequency: " << fNatrual << std::endl;
    std::cout << "Real frequency(estimated): " << fReal << std::endl;
    std::cout << "Stiffness: " << stiffness << std::endl;
    std::cout << "Damping: " << damping << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
}
