#pragma once
#include <vector>
#include <functional>
using StepCallback = std::function<void(const std::vector<double>&)>;
class RungeKutta4
{

public:
	int dimension;
	double stepSize;
	int numSteps;
	std::vector<std::function<double(const std::vector<double>&)>> functions;
	RungeKutta4(int dim, double h, int steps, const std::vector<std::function<double(const std::vector<double>&)>>& funcs)
		: dimension(dim), stepSize(h), numSteps(steps), functions(funcs) {
	}
	void setStepFunction(const StepCallback& func) {
		stepFunction = func;
	}
	
	void integrate(std::vector<double>& state);
private:

	StepCallback stepFunction = [](const std::vector<double>& state) { return; };
};

