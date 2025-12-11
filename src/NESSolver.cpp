#include "NESSolver.h"

NESSolver::NESSolver(unsigned int nesNumber_):
nesNumber(nesNumber_),
dimension(3 + 2 * nesNumber_)
{

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