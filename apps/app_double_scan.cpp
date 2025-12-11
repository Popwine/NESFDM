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
#define PI 	3.14159265358979323846

bool isEQ(double a, double b) {
	return std::abs(a - b) < 1e-10;
}


double getRms(const std::vector<std::vector<double>>& data,
	size_t index, double startTime = 0.0)
{
	if (data.empty() || index >= data[0].size()) return 0.0;

	double dt = data[1][0] - data[0][0];
	size_t startIndex = static_cast<size_t>(startTime / dt);

	if (startIndex >= data.size()) return 0.0;

	double sum = 0.0;
	size_t count = data.size();

	for (size_t i = startIndex; i < count; i++) {
		sum += data[i][index] * data[i][index];
	}

	return std::sqrt(sum / (count - startIndex));
}
double getMax(const std::vector<std::vector<double>>& data, size_t index, double startTime = 0.0) {
	if (data.empty() || index >= data[0].size()) return 0.0;
	double dt = data[1][0] - data[0][0];
	size_t startIndex = static_cast<size_t>(startTime / dt);
	if (startIndex >= data.size()) return 0.0;
	double maxVal = std::numeric_limits<double>::lowest();
	for (size_t i = startIndex; i < data.size(); i++) {
		if (data[i][index] > maxVal) {
			maxVal = data[i][index];
		}
	}
	return maxVal;
}

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
struct Config {
	// --- 必填参数 ---
	double mr1 = 0.0; bool has_mr1 = false;
	double mr2 = 0.0; bool has_mr2 = false;
	double kr1 = 0.0; bool has_kr1 = false;
	double kr2 = 0.0; bool has_kr2 = false;
	double cr1 = 0.0; bool has_cr1 = false;
	double cr2 = 0.0; bool has_cr2 = false;

	// --- 选填参数 (带默认值) ---
	double a = 0.01;
	double ts = 0.001;
	double ct = 500.0;
	double fd = 1.117;
	double fn = 1.117;
	double ur = 1.7;
	std::string config = "";
	std::string objective_function = "max_avg";
	// 特殊处理：rct 的默认值依赖于 ct
	// 我们使用一个标志位来判断用户是否输入了该值
	double rct = 0.0;
	bool has_rct = false;

	std::string save = "";
};
void get_avg_max(const std::vector<DisplacementResults>& allResults, double& jYRms, double& jYMax) {
	jYRms = 0.0;
	jYMax = 0.0;
	std::function<double(double, double, double)> max = [](double a, double b, double c) {
		return std::max(std::max(a, b), c);
		};
	double max1 = max(
		allResults[0].yRms,
		allResults[1].yRms,
		allResults[2].yRms
	);
	double max2 = max(
		allResults[3].yRms,
		allResults[4].yRms,
		allResults[5].yRms
	);
	double max3 = max(
		allResults[6].yRms,
		allResults[7].yRms,
		allResults[8].yRms
	);
	jYRms = (max1 + max2 + max3) / 3.0;
	double maxY1 = max(
		allResults[0].yMax,
		allResults[1].yMax,
		allResults[2].yMax
	);
	double maxY2 = max(
		allResults[3].yMax,
		allResults[4].yMax,
		allResults[5].yMax
	);
	double maxY3 = max(
		allResults[6].yMax,
		allResults[7].yMax,
		allResults[8].yMax
	);
	jYMax = (maxY1 + maxY2 + maxY3) / 3.0;
}
int main(int argc, char* argv[]){
    CLI::App app{"Double NES Scanner"};
    double initialAStar = 0.01;
    // tao = f_n * t
    double totalTao = 500;
    double resultCalcStartTao = 250;
    
    app.add_option("-a,--initial-a-star", initialAStar, "Initial AStar");
    app.add_option("-a,--initial-a-star", initialAStar, "Initial AStar");
    CLI11_PARSE(app, argc, argv);
    std::cout << initialAStar;
    return 0;
}