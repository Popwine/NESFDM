#include "RungeKutta4.h"
void RungeKutta4::integrate(std::vector<double>& state) {
	stepFunction(state);
	std::vector<double> k1(dimension), k2(dimension), k3(dimension), k4(dimension), tempState(dimension);
	for (int step = 0; step < numSteps; ++step) {
		
		// Compute k1
		for (int i = 0; i < dimension; ++i) {
			k1[i] = stepSize * functions[i](state);
		}
		// Compute k2
		for (int i = 0; i < dimension; ++i) {
			tempState[i] = state[i] + 0.5 * k1[i];
		}
		for (int i = 0; i < dimension; ++i) {
			k2[i] = stepSize * functions[i](tempState);
		}
		// Compute k3
		for (int i = 0; i < dimension; ++i) {
			tempState[i] = state[i] + 0.5 * k2[i];
		}
		for (int i = 0; i < dimension; ++i) {
			k3[i] = stepSize * functions[i](tempState);
		}
		// Compute k4
		for (int i = 0; i < dimension; ++i) {
			tempState[i] = state[i] + k3[i];
		}
		for (int i = 0; i < dimension; ++i) {
			k4[i] = stepSize * functions[i](tempState);
		}
		// Update state
		for (int i = 0; i < dimension; ++i) {
			state[i] += (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]) / 6.0;
		}
		// Call the step function
		stepFunction(state);
	}
}