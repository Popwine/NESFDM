#include <iostream>

#include "NESSolver.h"

int main(int argc, char* argv[]){
    MainStructure main;
    main.print();
    main.setFNByMode(3);
    main.setUStar(1.8);
    main.print();
    return 0;
}