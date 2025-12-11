#include "NESSolver.h"
#include "NESFDMUtils.h"
#include <math.h>

NESSolver::NESSolver(const unsigned int nesNumber_):
nesNumber(nesNumber_),
dimension(3 + 2 * nesNumber_)
{
    nes.resize(nesNumber);
    setMr.resize(nesNumber);
    setKr.resize(nesNumber);
    setCr.resize(nesNumber);
    for(int i = 0; i < nesNumber; i++){
        setMr[i] = [this, i](double mr_){
            this->nes[i].mr = mr_;
        };
        setKr[i] = [this, i](double kr_){
            this->nes[i].kr = kr_;
        };
        setCr[i] = [this, i](double cr_){
            this->nes[i].cr = cr_;
        };
    }

    funcs.resize(dimension);
    // function 0 t
    funcs[0] = [](const std::vector<double>& state) { return 1; }; // t
    // functions  yp, ya1, ya2... yan
    // index      1,  2,   3,     n+1
    // return     n+2,n+3, n+4,   2n+2

    for(int i = 1; i <= nesNumber + 1; i++){
        funcs[i] = [this, i](const std::vector<double>& state) { return state[i + nesNumber + 1]; }; 
    }

    
    

}

NESSolver::~NESSolver(){

}
MainStructure::MainStructure(){
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
void MainStructure::print(){
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

}
