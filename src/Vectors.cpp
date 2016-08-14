#include "Vectors.h"

template<typename T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& v){
	if (v.size() == 0)
		os << "{}";
	else{
		os << '{' << v[0];
		for (size_t i = 1; i < v.size(); i++)
			os << ',' << v[i];
		os << '}';
	}
	return os;
}
template std::ostream& operator<< <double>(std::ostream& os, const std::vector<double>& v);
template std::ostream& operator<< <int>(std::ostream& os, const std::vector<int>& v);
template std::ostream& operator<< <unsigned int>(std::ostream& os, const std::vector<unsigned int>& v);


std::vector<double> operator+(const std::vector<double>& a, const std::vector<double>& b){
	assert(a.size() == b.size());
	std::vector<double>c;
	for (size_t i = 0; i < a.size(); i++)c.push_back(a[i] + b[i]);
	return c;
}
std::vector<double> operator+=(std::vector<double>& a, const std::vector<double>& b){
	assert(a.size() == b.size());
	for (size_t i = 0; i < a.size(); i++)a[i] += b[i];
	return a;
}

std::vector<double> operator-(const std::vector<double>& a, const std::vector<double>& b){
	assert(a.size() == b.size());
	std::vector<double>c;
	for (size_t i = 0; i < a.size(); i++)c.push_back(a[i] - b[i]);
	return c;
}
std::vector<double> operator-=(std::vector<double>& a, const std::vector<double>& b){
	assert(a.size() == b.size());
	for (size_t i = 0; i < a.size(); i++)a[i] -= b[i];
	return a;
}

std::vector<double> operator-(const std::vector<double>& a){
	std::vector<double>c;
	for (size_t i = 0; i < a.size(); i++)c.push_back(-a[i]);
	return c;
}

std::vector<double> operator+(double d, const std::vector<double>& a){
	std::vector<double>c;
	for (size_t i = 0; i < a.size(); i++)c.push_back(d + a[i]);
	return c;
}
std::vector<double> operator+(const std::vector<double>& a, double d){
	std::vector<double>c;
	for (size_t i = 0; i < a.size(); i++)c.push_back(a[i] + d);
	return c;
}
std::vector<double> operator-(double d, const std::vector<double>& a){
	std::vector<double>c;
	for (size_t i = 0; i < a.size(); i++)c.push_back(d - a[i]);
	return c;
}
std::vector<double> operator-(const std::vector<double>& a, double d){
	std::vector<double>c;
	for (size_t i = 0; i < a.size(); i++)c.push_back(a[i] - d);
	return c;
}

std::vector<double> operator*(double d, std::vector<double> a){
	for (size_t i = 0; i < a.size(); i++)a[i] *= d;
	return a;
}
std::vector<double> operator*(std::vector<double> a, double d){
	for (size_t i = 0; i < a.size(); i++)a[i] *= d;
	return a;
}

double dotProduct(const std::vector<double>& a, const std::vector<double>& b){
	assert(a.size() == b.size());
	return std::inner_product(a.begin(), a.end(), b.begin(), 0.0);
}

std::vector<double> hadamardProduct(const std::vector<double>& a, const std::vector<double>& b){
	assert(a.size() == b.size());
	std::vector<double> c;
	for (size_t i = 0; i < a.size(); i++)
		c.push_back(a[i] * b[i]);
	return c;
}

template<typename T> std::vector<T> extractIndices(std::vector<T> oldvec, std::vector<size_t> indices){
	std::vector<T> newvec();
	newvec.reserve(indices.size());
	for (size_t ind : indices){
		assert(ind < oldvec.size());
		newvec.push_back(oldvec[ind]);
	}
	return newvec;
}

