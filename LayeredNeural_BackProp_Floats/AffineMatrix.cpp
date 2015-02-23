#include "AffineMatrix.h"

template<typename T> AffineMatrix<T>::AffineMatrix(size_t h, size_t w){
	reserve_size(h, w);
	matrix.resize(height*width);
	biases.resize(height);
}
template<typename T> AffineMatrix<T>::AffineMatrix(size_t h, size_t w, T(*_fill_callback)()){
	reserve_size(h, w);
	for (size_t i = 0; i < height * width; i++)matrix.push_back(_fill_callback());
	for (size_t i = 0; i < height; i++)biases.push_back(_fill_callback());
}
template<typename T> AffineMatrix<T>::AffineMatrix(size_t h, size_t w, std::function<T(const size_t, const size_t)> _fill_callback, std::function<T(const size_t)> _bias_callback){
	reserve_size(h, w);
	for (size_t i = 0; i < height * width; i++)matrix.push_back(_fill_callback(i / width, i%width));
	for (size_t i = 0; i < height; i++)biases.push_back(_bias_callback(i));
}

template<typename T> AffineMatrix<T>::AffineMatrix(std::vector<T> mdata, std::vector<T> bdata){
	reserve_size(bdata.size(), mdata.size() / bdata.size());

	assert(mdata.size() == width*height);
	assert(bdata.size() == height);
	matrix = mdata;
	biases = bdata;
}
template<typename T> AffineMatrix<T>::AffineMatrix(std::vector<std::vector<T> > mdata, std::vector<T> bdata){
	reserve_size(mdata.size(), mdata[0].size());

	for (std::vector<T> v : mdata){
		assert(v.size() == width);
		for (T a : v)
			matrix.push_back(a);
	}

	assert(bdata.size() == height);
	biases = bdata;
}

template<typename T> void AffineMatrix<T>::reserve_size(size_t h, size_t w){
	assert(matrix.size() == 0 && biases.size() == 0);
	height = h;
	width = w;
	matrix.reserve(height * width);
	biases.reserve(height);
}

template<typename T> std::vector<T> AffineMatrix<T>::operator*(const std::vector<T>& v) const{
	assert(v.size() == width);
	assert(biases.size() == height);
	std::vector<T> a(height, 0);
	for (size_t i = 0; i < height; i++){
		for (size_t j = 0; j < width; j++)
			a[i] += v[j] * matrix[i*width + j];
		a[i] += biases[i];
	}
	return a;
}
template<typename T> std::vector<T> AffineMatrix<T>::transposeMultiply(const std::vector<T>& v) const{
	//Ignores biases.
	assert(v.size() == height);
	std::vector<T> a(width, 0);
	for (size_t i = 0; i < width; i++)
		for (size_t j = 0; j < height; j++)
			a[i] += v[j] * matrix[i + j*width];
	return a;
}
template<typename T> std::vector<T> AffineMatrix<T>::operator[](size_t i) const{
	assert(i < height);
	return std::vector<T>(matrix.begin() + i*width, matrix.begin() + (i + 1)*width); //Apparently upper bound is exclusive in range iterator.
}
template<typename T> AffineMatrix<T> AffineMatrix<T>::operator+(const AffineMatrix<T>& other) const{
	assert(height == other.height && width == other.width);
	return AffineMatrix<T>(matrix + other.matrix, biases + other.biases);
}
template<typename T> AffineMatrix<T> AffineMatrix<T>::operator+=(const AffineMatrix<T>& other){
	assert(height == other.height && width == other.width);
	matrix += other.matrix;
	biases += other.biases;
	return *this;
}
template<typename T> AffineMatrix<T> AffineMatrix<T>::operator-=(const AffineMatrix<T>& other){
	assert(height == other.height && width == other.width);
	matrix -= other.matrix;
	biases -= other.biases;
	return *this;
}
template<typename T> void AffineMatrix<T>::b(std::vector<T> newbiases){
	assert(newbiases.size() == height);
	biases = newbiases;
}


template<typename T> AffineMatrix<T> AffineMatrix<T>::mutate(double range) const{
	std::vector<T> newmatrix = matrix;
	for (T &v : newmatrix)
		v += rand11() * range;

	std::vector<T> newbiases = biases;
	for (T &v : newbiases)
		v += rand11() * range;

	return AffineMatrix<T>(newmatrix, newbiases);
}
template<typename T> AffineMatrix<T> AffineMatrix<T>::hybridize(AffineMatrix<T> other) const{
	assert(size_equals(other));

	std::vector<T> newmatrix = matrix;
	for (size_t i = 0; i < newmatrix.size(); i++){
		double prop = rand01();
		newmatrix[i] = prop * newmatrix[i] + (1 - prop)*other.matrix[i];
	}

	std::vector<T> newbiases = biases;
	for (size_t i = 0; i < newbiases.size(); i++){
		double prop = rand01();
		newbiases[i] = prop * newbiases[i] + (1 - prop)*other.biases[i];
	}

	return AffineMatrix<T>(newmatrix, newbiases);
}

template<typename T> void AffineMatrix<T>::callback(T(*_fill_callback)()){
	for (size_t i = 0; i < height * width; i++)matrix[i] = _fill_callback();
	for (size_t i = 0; i < height; i++)biases[i] = _fill_callback();
}
template<typename T> void AffineMatrix<T>::callback(std::function<void(const size_t, const size_t, double&)> _fill_callback, std::function<void(const size_t, double&)> _bias_callback){
	for (size_t i = 0; i < height * width; i++)_fill_callback(i / width, i%width, matrix[i]);
	for (size_t i = 0; i < height; i++)_bias_callback(i, biases[i]);
}


std::ostream& operator<< (std::ostream& os, const AffineMatrix<double>& a){
	for (size_t i = 0; i < a.h(); i++)
		os << a[i] << std::endl;
	os << "BIAS: " << a.b();
	return os;
}

AffineMatrix<double> affineOuterProduct(std::vector<double> a, std::vector<double> b){
	return AffineMatrix<double>(a.size(), b.size(), [a, b](size_t i, size_t j){return a[i] * b[j]; }, [a, b](size_t i){return a[i]; });
}



template class AffineMatrix<double>;
//template class AffineMatrix<float>;
//template class AffineMatrix<int>;

