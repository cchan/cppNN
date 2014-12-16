#include "Vectors.h"

std::ostream& operator<< (std::ostream& os, const std::vector<double>& v){
	if (v.size() == 0)os << "{}";
	os << '{' << v[0];
	for (size_t i = 1; i < v.size(); i++)
		os << ',' << v[i];
	os << '}';
	return os;
}

std::vector<double> operator+(std::vector<double> a, const std::vector<double>& b){
	for (size_t i = 0; i < a.size(); i++)a[i] += b[i];
	return a;
}
std::vector<double> operator-(std::vector<double> a, const std::vector<double>& b){
	for (size_t i = 0; i < a.size(); i++)a[i] -= b[i];
	return a;
}
std::vector<double> operator-(std::vector<double> a){
	for (size_t i = 0; i < a.size(); i++)a[i] = -a[i];
	return a;
}
std::vector<double> operator*(double d, std::vector<double> a){
	for (size_t i = 0; i < a.size(); i++)a[i] *= d;
	return a;
}
std::vector<double> operator*(std::vector<double> a, double d){
	for (size_t i = 0; i < a.size(); i++)a[i] *= d;
	return a;
}
std::vector<double> operator+=(std::vector<double>& a, const std::vector<double>& b){
	for (size_t i = 0; i < a.size(); i++)a[i] += b[i];
	return a;
}
std::vector<double> operator-=(std::vector<double>& a, const std::vector<double>& b){
	for (size_t i = 0; i < a.size(); i++)a[i] -= b[i];
	return a;
}
double dotProduct(const std::vector<double>& a, const std::vector<double>& b){
	return std::inner_product(a.begin(), a.end(), b.begin(), 0.0);
}
std::vector<double> hadamardProduct(std::vector<double> a, const std::vector<double>& b){
	for (size_t i = 0; i < a.size(); i++)
		a[i] *= b[i];
	return a;
}