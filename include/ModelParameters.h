#pragma once
#include <vector>
#include <string>
struct SingleParameter {
	double A_star;
	double U_star;
	double H1_star;
	double H4_star;
	double slope_H1 = 0.0;
	double slope_H4 = 0.0;
	SingleParameter(double a, double u, double h1, double h4)
        : A_star(a), U_star(u), H1_star(h1), H4_star(h4) {}
};
class ModelParameters
{
public:

	ModelParameters(double U_star_, std::string modelParamFile);
	ModelParameters(double U_star_);
	double getUStar() const { return U_star; }
	void getAeroCoeffs(double A_star, double& h1_out, double& h4_out) const;

private:
	double U_star;
	bool isValidDouble(const std::string& s, double& value);
	std::vector<SingleParameter> parameters;
};