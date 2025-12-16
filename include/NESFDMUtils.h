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
	void print() const {
		std::cout << std::setprecision(10) << yRms << "\t" << yMax << std::endl;
	}
	void printRms() const {
		std::cout << std::setprecision(10) << yRms << std::endl;
	}

};
void get_avg_max(const std::vector<DisplacementResults>& allResults, double& jYRms, double& jYMax);