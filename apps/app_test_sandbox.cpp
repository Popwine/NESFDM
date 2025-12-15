#include <iostream>

#include "NESSolver.h"

int main(int argc, char* argv[]){
    try{
        auto solver = NESSolver(2);
        solver.setNESMr(1, 0.01);
        solver.setNESMr(2, 0.005);
        solver.setNESKr(1, 0.55);
        solver.setNESCr(1, 0.65);
        solver.setNESKr(2, 1.0);
        solver.setNESCr(2, 0.003);
        solver.setInitialAStar(0.01);
        solver.setOutput("stateTimeHistory.txt");
        solver.refreshAll();
        solver.printAll();

        for(const auto& f : solver.funcs){
            std::cout << f({ 1.0,2.0,3.0,4.0,5.0,6.0,7.0 }) << std::endl;
        }
        auto result = solver.run();
        result.print(0.0532);
    }
    catch(const std::exception& e){
        std::cerr << "Parsing Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}