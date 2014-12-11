#pragma once
#include <vector>
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
	AffineMatrix(size_t h, size_t w, T(*fill_callback)());
	AffineMatrix(std::vector<std::vector<T> > mdata, std::vector<T> bdata);

	size_t h() const{ return height; };
	std::vector<T> operator*(std::vector<T> v) const;
	std::vector<T> transposeMultiply(std::vector<T> v) const;
	std::vector<T> operator[](size_t i) const;
	std::vector<T> b() const{ return biases; }
	void b(std::vector<T> newbiases);
	void callback(std::function<void(const size_t, const size_t, T&)> c);
};

std::ostream& operator<< (std::ostream& os, const AffineMatrix<double>& a);
