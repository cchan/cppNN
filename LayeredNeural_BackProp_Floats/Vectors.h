#pragma once
#include <vector>
#include <numeric>
#include <ostream>

std::ostream& operator<< (std::ostream& os, const std::vector<double>& v);
std::vector<double> operator+(std::vector<double> a, const std::vector<double>& b);
std::vector<double> operator-(std::vector<double> a, const std::vector<double>& b);
std::vector<double> operator-(std::vector<double> a);
std::vector<double> operator*(double d, std::vector<double> a);
std::vector<double> operator*(std::vector<double> a, double d);
std::vector<double> operator+=(std::vector<double>& a, const std::vector<double>& b);
std::vector<double> operator-=(std::vector<double>& a, const std::vector<double>& b);
double dotProduct(const std::vector<double>& a, const std::vector<double>& b);
std::vector<double> hadamardProduct(std::vector<double> a, const std::vector<double>& b);


