#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <chrono>
#include <algorithm>
#include "CLI11.hpp"
#include "RungeKutta4.h"
#include "ModelParameters.h"
#include "NESFDMUtils.h"


DisplacementResults integrateDoubleNes(
	double U_star = 1.7,
	double mr1 = 0.010,
	double mr2 = 0.005,
	double ka1 = 165614.078216182,
	double ka2 = 165614.078216182,
	double ca1 = 0.308244526038213,
	double ca2 = 0.308244526038213,
	double initialA_star = 0.06,
	double fNatrual = 1.117,
	double timeStepSize = 0.001,
	double totalTime = 500,
	double resultCalcStartTime = 100.0,
	std::string outputFileName = "output_double_nes.txt"
) {






	double mass = 7.32; // mass of the particle
	double fRealFactors[3] = { 0.968468691, 0.973652933, 0.979006853 };
	double dampingRatio = 0.003;
	double D = 0.0532;
	double B = 0.7117;
	double rou = 1.225;


	int dimension = 7;


	double fRealList[3] = { 0.0 };
	for (int i = 0; i < 3; i++) {
		fRealList[i] = fRealFactors[i] * fNatrual;
	}

	double ma1 = mr1 * mass;
	double ma2 = mr2 * mass;
	double fReal;
	if (isEQ(U_star, 1.6)) {
		fReal = fRealList[0];
	}
	else if (isEQ(U_star, 1.7)) {
		fReal = fRealList[1];
	}
	else if (isEQ(U_star, 1.8)) {
		fReal = fRealList[2];
	}
	else {
		throw std::runtime_error("Unsupported U* value.");
	}
	//double U = U_star * fReal * B;
	double stiffness = mass * (2 * PI * 1.0 * fNatrual) * (2 * PI * 1.0 * fNatrual);

	double damping = 2 * dampingRatio * sqrt(stiffness * mass);

	//double y1DotFactor = 0.5 * rou * U * U * B * (2 * PI / U_star) / U;
	double y1DotFactor = PI * rou * B * fReal * B;
	//double y1Factor = 0.5 * rou * U * U * B * (2 * PI / U_star) * (2 * PI / U_star) / D;
	double y1Factor = 2 * PI * PI * rou * B * B * B * fReal * fReal / D;
	double omega = fReal * 2 * PI;

	ModelParameters modelParameters(U_star, "ModelParameters.txt");







	std::vector<std::function<double(const std::vector<double>&)>> singleNesFunctions(7);
	singleNesFunctions[0] = [](const std::vector<double>& state) { return 1; }; // t
	singleNesFunctions[1] = [](const std::vector<double>& state) { return state[4]; };
	singleNesFunctions[2] = [](const std::vector<double>& state) { return state[5]; };
	singleNesFunctions[3] = [](const std::vector<double>& state) { return state[6]; };
	singleNesFunctions[4] = [
		stiffness, mass, damping, y1DotFactor, y1Factor, D, omega, &modelParameters,
			ka1, ka2, ca1, ca2
	](const std::vector<double>& state) {
		double fl;
		double y = state[1];
		double yv = state[4];
		double current_A_star = std::sqrt(y * y + yv * yv / omega / omega) / D;
		double h1, h4;
		modelParameters.getAeroCoeffs(current_A_star, h1, h4);
		fl = y1DotFactor * h1 * yv + y1Factor * h4 * y;

		return (
			-damping * state[4] - stiffness * state[1]
			+ ca1 * (state[5] - state[4])
			+ ka1 * (state[2] - state[1]) * (state[2] - state[1]) * (state[2] - state[1])
			+ ca2 * (state[6] - state[4])
			+ ka2 * (state[3] - state[1]) * (state[3] - state[1]) * (state[3] - state[1])
			+ fl
			) / mass;


		};

	singleNesFunctions[5] = [
		mass, ka1, ka2, ca1, ca2, ma1
	](const std::vector<double>& state) {

		return (
			ca1 * (state[4] - state[5])
			+ ka1 * (state[1] - state[2]) * (state[1] - state[2]) * (state[1] - state[2])
			) / ma1;


		};

	singleNesFunctions[6] = [
		mass, ka1, ka2, ca1, ca2, ma2
	](const std::vector<double>& state) {

		return (
			ca2 * (state[4] - state[6])
			+ ka2 * (state[1] - state[3]) * (state[1] - state[3]) * (state[1] - state[3])
			) / ma2;


		};

	int numSteps = static_cast<int>(totalTime / timeStepSize);

	RungeKutta4 rk4(dimension, timeStepSize, numSteps, singleNesFunctions);
	std::vector<double> state = { 0.0, initialA_star * D, initialA_star * D, initialA_star * D, 0.0, 0.0, 0.0 };

	//RungeKutta4 rk4(3, 0.001, 200000, freeFunctions);
	//std::vector<double> state = { 0.0, 0.01 * D, 0.0};

	std::ofstream ofs(outputFileName);

	std::vector<std::vector<double>> results;
	std::function<void(const std::vector<double>&)> stepFunction;
	if (!outputFileName.empty()) {
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
	double yRms = getRms(results, 1, resultCalcStartTime);
	double yMax = getMax(results, 1, resultCalcStartTime);
	return DisplacementResults{ yRms,yMax };
}
int main(int argc, char* argv[]){
    CLI::App app{"Double NES Scanner"};
    double initialAStar = 0.01;
    // tao = f_n * t
    double totalTao = 500;
    double resultCalcStartTao = 250;
    
    app.add_option("-a,--initial-a-star", initialAStar, "Initial AStar");
    app.add_option("-rct,--result-calc-start-tao", resultCalcStartTao, "Result Calculation Start Time");
    CLI11_PARSE(app, argc, argv);
    std::cout << initialAStar;
    return 0;
}