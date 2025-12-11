
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <chrono>
#include <algorithm>
#include <functional>
#include <math.h>
#include "NESFDMUtils.h"
bool isEQ(double a, double b) {
	return std::abs(a - b) < 1e-10;
}


double getRms(const std::vector<std::vector<double>>& data,
	size_t index, double startTime)
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
double getMax(const std::vector<std::vector<double>>& data, size_t index, double startTime) {
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