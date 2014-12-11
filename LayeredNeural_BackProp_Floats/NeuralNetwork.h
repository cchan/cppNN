#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include "AffineMatrix.h"

class NeuralNetwork{
private:
	size_t inputSize;
	std::vector<size_t> sizes;
	std::vector<std::vector<double> > prevActivations;
public:
	std::vector<AffineMatrix<double> > nlayers;
	NeuralNetwork(const size_t is, const std::vector<size_t>& s, double(*weightsInit)());
	void backprop(std::vector<double> outputdelta);
	std::vector<double> frontprop(std::vector<double> input);
	std::vector<double> thresholding(std::vector<double> v);
	std::vector<double> thresholding_prime(std::vector<double> v);
};