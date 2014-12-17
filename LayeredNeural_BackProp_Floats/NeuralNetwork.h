#pragma once
#include <functional>
#include <algorithm>
#include "Vectors.h"
#include "AffineMatrix.h"
#include "rand11.h"

class NeuralNetwork{
private:
	size_t inputSize;
	std::vector<size_t> sizes;
	double learningRate, annealingRate, weightmax;
	std::vector<AffineMatrix<double> > nlayers;
public:
	NeuralNetwork(const size_t is, const std::vector<size_t>& s, double ar);
	AffineMatrix<double> operator[](size_t i){ return nlayers[i]; }
	
	void randInit();
	std::vector<double> frontprop(std::vector<double> input);

	//Genetic stuff
	NeuralNetwork getRandomlyMutated(double range) const;
	NeuralNetwork getRandomlyHybridized(const NeuralNetwork& n) const;

	//Backprop stuff
	std::vector<double> backprop(std::vector<double> testdata, std::vector<double> correct);
	std::vector<double> thresholding(std::vector<double> v);
	std::vector<double> thresholding_prime(std::vector<double> v);
};