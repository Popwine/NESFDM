#include "ModelParameters.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
bool ModelParameters::isValidDouble(const std::string& s, double& value) {
	std::istringstream iss(s);
	iss >> value;
	return !iss.fail() && iss.eof(); // 检查是否完全解析为double
}
ModelParameters::ModelParameters(double U_star_, std::string modelParamFile) : U_star(U_star_) {
	auto ifs = std::ifstream(modelParamFile);

	if (!ifs.is_open()) {

		throw std::runtime_error("Can not open model parameters file");
	}
	std::string line;
	int lineNumber = 0;
	while (std::getline(ifs, line)) {
		lineNumber++;
		if (line.empty()) continue;
		std::istringstream iss(line);
		std::string token;
		std::vector<double> values;
		while (iss >> token) {
			double val;
			if (!isValidDouble(token, val)) {

				throw std::runtime_error(std::string("Invalid data in line ") + std::to_string(lineNumber));

				break;
			}
			values.push_back(val);
		}
		if (values.size() != 4) {

			throw std::runtime_error(std::string("Invalid number of columns in line ") + std::to_string(lineNumber));
			continue;
		}

		SingleParameter param{ values[0], values[1], values[2], values[3] };
		if (param.U_star < U_star_ * 1.00000001 && param.U_star > U_star_ * 0.99999999) {
			parameters.push_back(param);
		}


	}
	ifs.close();
	std::sort(parameters.begin(), parameters.end(), [](const SingleParameter& a, const SingleParameter& b) {
		return a.A_star < b.A_star; // 升序
		});
	if (parameters.size() < 2) {
		throw std::runtime_error("Not enough data points for the specified U*");
	}
	//打印数据
	//for (const auto& p : parameters) {
	//	std::cout << "A*: " << p.A_star << ", U*: " << p.U_star << ", H1*: " << p.H1_star << ", H4*: " << p.H4_star << std::endl;
	//}
	// 预计算斜率
	for (size_t i = 0; i < parameters.size() - 1; ++i) {
		double dA = parameters[i + 1].A_star - parameters[i].A_star;
		// 防止 dA 为 0 (虽然理论上不应该发生)
		if (std::abs(dA) < 1e-9) dA = 1.0;

		parameters[i].slope_H1 = (parameters[i + 1].H1_star - parameters[i].H1_star) / dA;
		parameters[i].slope_H4 = (parameters[i + 1].H4_star - parameters[i].H4_star) / dA;
	}
	// 最后一个点的斜率设为0（反正也不会用到，因为会被右边界截断拦截）
	parameters.back().slope_H1 = 0.0;
	parameters.back().slope_H4 = 0.0;
}
ModelParameters::ModelParameters(double U_star_) {

	if (U_star_ < 1.6 * 1.00000001 && U_star_ > 1.6 * 0.99999999){
		parameters.emplace_back(0.01, 1.6, 1.0125494, 0.0937759);
		parameters.emplace_back(0.02, 1.6, 1.0799486, 0.1095458);
		parameters.emplace_back(0.03, 1.6, 1.0310618, 0.1246971);
		parameters.emplace_back(0.04, 1.6, 0.9159057, 0.1322624);
		parameters.emplace_back(0.05, 1.6, 0.7910705, 0.1327791);
		parameters.emplace_back(0.06, 1.6, 0.654112, 0.1292456);
		parameters.emplace_back(0.07, 1.6, 0.4980179, 0.1245232);
		parameters.emplace_back(0.08, 1.6, 0.3305332, 0.1202329);
		parameters.emplace_back(0.09, 1.6, 0.1632002, 0.1169685);
		parameters.emplace_back(0.10, 1.6, 0.0097263, 0.1157514);
		parameters.emplace_back(0.11, 1.6, -0.1100127, 0.1153979);
		parameters.emplace_back(0.12, 1.6, -0.1973558, 0.1145267);
		parameters.emplace_back(0.13, 1.6, -0.2642202, 0.1130295);
	}
	else if (U_star_ < 1.7 * 1.00000001 && U_star_ > 1.7 * 0.99999999){
		parameters.emplace_back(0.01, 1.7, 0.6242359, 0.0582735);
		parameters.emplace_back(0.02, 1.7, 0.7259674, 0.0636135);
		parameters.emplace_back(0.03, 1.7, 0.7792199, 0.0698904);
		parameters.emplace_back(0.04, 1.7, 0.7885622, 0.076962);
		parameters.emplace_back(0.05, 1.7, 0.7586608, 0.0826012);
		parameters.emplace_back(0.06, 1.7, 0.6944979, 0.0861984);
		parameters.emplace_back(0.07, 1.7, 0.6000097, 0.0883548);
		parameters.emplace_back(0.08, 1.7, 0.4819205, 0.0901685);
		parameters.emplace_back(0.09, 1.7, 0.3506624, 0.092304);
		parameters.emplace_back(0.10, 1.7, 0.2188693, 0.0950711);
		parameters.emplace_back(0.11, 1.7, 0.0973057, 0.0986067);
		parameters.emplace_back(0.12, 1.7, -0.0040616, 0.1018806);
		parameters.emplace_back(0.13, 1.7, -0.0841328, 0.1039627);
	}
	else if (U_star_ < 1.8 * 1.00000001 && U_star_ > 1.8 * 0.99999999){
		parameters.emplace_back(0.01, 1.8, 0.2996206, 0.043606);
		parameters.emplace_back(0.02, 1.8, 0.3604417, 0.0440432);
		parameters.emplace_back(0.03, 1.8, 0.4215997, 0.0453345);
		parameters.emplace_back(0.04, 1.8, 0.4639818, 0.0481981);
		parameters.emplace_back(0.05, 1.8, 0.4826795, 0.0519661);
		parameters.emplace_back(0.06, 1.8, 0.4769947, 0.0556499);
		parameters.emplace_back(0.07, 1.8, 0.4485873, 0.0590451);
		parameters.emplace_back(0.08, 1.8, 0.3989758, 0.0623673);
		parameters.emplace_back(0.09, 1.8, 0.332252, 0.065979);
		parameters.emplace_back(0.10, 1.8, 0.2545708, 0.0701668);
		parameters.emplace_back(0.11, 1.8, 0.1731251, 0.0750515);
		parameters.emplace_back(0.12, 1.8, 0.0940672, 0.0805768);
		parameters.emplace_back(0.13, 1.8, 0.0231755, 0.0860524);
	}
	else{
		throw std::runtime_error("Unsupported U*.");
	}
	std::sort(parameters.begin(), parameters.end(), [](const SingleParameter& a, const SingleParameter& b) {
		return a.A_star < b.A_star; // 升序
	});
	if (parameters.size() < 2) {
		throw std::runtime_error("Not enough data points for the specified U*.");
	}
	//打印数据
	//for (const auto& p : parameters) {
	//	std::cout << "A*: " << p.A_star << ", U*: " << p.U_star << ", H1*: " << p.H1_star << ", H4*: " << p.H4_star << std::endl;
	//}
	// 预计算斜率
	for (size_t i = 0; i < parameters.size() - 1; ++i) {
		double dA = parameters[i + 1].A_star - parameters[i].A_star;
		// 防止 dA 为 0 (虽然理论上不应该发生)
		if (std::abs(dA) < 1e-9) dA = 1.0;

		parameters[i].slope_H1 = (parameters[i + 1].H1_star - parameters[i].H1_star) / dA;
		parameters[i].slope_H4 = (parameters[i + 1].H4_star - parameters[i].H4_star) / dA;
	}
	// 最后一个点的斜率设为0（反正也不会用到，因为会被右边界截断拦截）
	parameters.back().slope_H1 = 0.0;
	parameters.back().slope_H4 = 0.0;
}
void ModelParameters::getAeroCoeffs(double A_star, double& h1_out, double& h4_out) const {


	// 左边界 (Small Amplitude)
	if (A_star <= parameters.front().A_star) {
		h1_out = parameters.front().H1_star;
		h4_out = parameters.front().H4_star;
		return;
	}

	// 右边界 (Large Amplitude)
	if (A_star >= parameters.back().A_star) {
		h1_out = parameters.back().H1_star;
		h4_out = parameters.back().H4_star;
		return;
	}

	// 2. 二分查找 (只做一次！)
	auto it = std::lower_bound(parameters.begin(), parameters.end(), A_star,
		[](const SingleParameter& p, double val) { return p.A_star < val; });

	// 3. 获取左侧点 (利用预计算的斜率)
	const auto& p1 = *(it - 1);
	double delta_A = A_star - p1.A_star;

	// 4. 并行计算两个值 (CPU流水线优化)
	// 现代CPU可以同时发射这两条指令
	h1_out = p1.H1_star + delta_A * p1.slope_H1;
	h4_out = p1.H4_star + delta_A * p1.slope_H4;
}
