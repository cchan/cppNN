#pragma once
#include <functional>
#include <cassert>
#include <ostream>
#include "common.h"
#include "Vectors.h"

template<typename T> class AffineMatrix{
public:
	size_t height;
	size_t width;
	std::vector<T> matrix;
	std::vector<T> biases;
	void reserve_size(size_t h, size_t w);
public:
	AffineMatrix(){}
	AffineMatrix(size_t h, size_t w);
	AffineMatrix(size_t h, size_t w, T(*fill_callback)());
	AffineMatrix(size_t h, size_t w, std::function<T(const size_t, const size_t)> fill_callback, std::function<T(const size_t)> bias_callback);
	AffineMatrix(std::vector<T> mdata, std::vector<T> bdata);
	AffineMatrix(std::vector<std::vector<T> > mdata, std::vector<T> bdata);

	size_t h() const{ return height; };
	size_t w() const{ return width; };
	std::vector<T> b() const{ return biases; }
	void b(std::vector<T> newbiases);

	std::vector<T> operator*(const std::vector<T>& v) const;
	std::vector<T> transposeMultiply(const std::vector<T>& v) const;
	std::vector<T> operator[](size_t i) const;
	AffineMatrix<T> operator+(const AffineMatrix<T>& other) const;
	AffineMatrix<T> operator+=(const AffineMatrix<T>& other);
	AffineMatrix<T> operator-=(const AffineMatrix<T>& other);
	bool operator==(const AffineMatrix<T>& other) const{ return matrix == other.matrix && biases == other.biases; };
	bool size_equals(const AffineMatrix<T>& other) const{ return height == other.height && width == other.width; };

	AffineMatrix<T> mutate(double range) const;
	AffineMatrix<T> hybridize(AffineMatrix<T> other) const;

	void callback(T(*_fill_callback)());
	void callback(std::function<void(const size_t, const size_t, double&)> _fill_callback, std::function<void(const size_t, double&)> _bias_callback);
};

std::ostream& operator<< (std::ostream& os, const AffineMatrix<double>& a);
AffineMatrix<double> affineOuterProduct(std::vector<double> a, std::vector<double> b);
template<typename T> AffineMatrix<T> operator*(T c, const AffineMatrix<T>& am){
	return AffineMatrix<T>(c*am.matrix, c*am.biases);
}

