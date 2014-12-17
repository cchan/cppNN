#pragma once
#include <functional>
#include <cassert>
#include <ostream>
#include "Vectors.h"

template<typename T> class AffineMatrix{
public:
	size_t height;
	size_t width;
	std::vector<T> matrix;
	std::vector<T> biases;
	void setsize(size_t h, size_t w);
public:
	AffineMatrix(){}
	AffineMatrix(size_t h, size_t w);
	AffineMatrix(size_t h, size_t w, T(*fill_callback)());
	AffineMatrix(size_t h, size_t w, std::function<T(const size_t, const size_t)> fill_callback, std::function<T(const size_t)> bias_callback);
	AffineMatrix(std::vector<T> mdata, std::vector<T> bdata);
	AffineMatrix(std::vector<std::vector<T> > mdata, std::vector<T> bdata);

	size_t h() const{ return height; };
	std::vector<T> operator*(std::vector<T> v) const;
	std::vector<T> transposeMultiply(std::vector<T> v) const;
	std::vector<T> operator[](size_t i) const;
	AffineMatrix<T> operator+(const AffineMatrix<T>& other) const;
	AffineMatrix<T> operator+=(const AffineMatrix<T>& other);
	AffineMatrix<T> operator-=(const AffineMatrix<T>& other);
	std::vector<T> b() const{ return biases; }
	void b(std::vector<T> newbiases);
	void callback(std::function<void(const size_t, const size_t, T&)> c);
};

std::ostream& operator<< (std::ostream& os, const AffineMatrix<double>& a);
AffineMatrix<double> affineOuterProduct(std::vector<double> a, std::vector<double> b);
template<typename T> AffineMatrix<T> operator*(T c, const AffineMatrix<T>& am){
	return AffineMatrix<T>(c*am.matrix, c*am.biases);
}
