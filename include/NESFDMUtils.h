#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <chrono>
#include <algorithm>
#define PI 	3.14159265358979323846
bool isEQ(double a, double b);
double getRms(const std::vector<std::vector<double>>& data, size_t index, double startTime = 0.0);
double getMax(const std::vector<std::vector<double>>& data, size_t index, double startTime = 0.0);
class DisplacementResults {
public:
	double yRms;
	double yMax;
	void print(double D) const {
		std::cout << std::setprecision(10) << yRms / D << "\t" << yMax / D << std::endl;
	}
	void printRms(double D) const {
		std::cout << std::setprecision(10) << yRms / D << std::endl;
	}

};
void get_avg_max(const std::vector<DisplacementResults>& allResults, double& jYRms, double& jYMax);