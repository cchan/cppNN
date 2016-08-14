#pragma once
#include <functional>
#include <algorithm>
#include <cassert>
#include "Vectors.h"
#include "AffineMatrix.h"
#include "common.h"
#include <sstream>


class NeuralNetwork{ // Guaranteed: no parallelization
private:
	size_t inputSize;
	std::vector<size_t> sizes;
	double learningRate, annealingRate;
	const double initialLearningRate = 0.1;
	std::vector<AffineMatrix<double> > nlayers;
public:
	NeuralNetwork(){};
	NeuralNetwork(const size_t is, const std::vector<size_t>& s, double ar = 1.0);
	NeuralNetwork(const NeuralNetwork& nn){ inputSize = nn.inputSize; sizes = nn.sizes; learningRate = nn.learningRate; annealingRate = nn.annealingRate; nlayers = nn.nlayers; }
	std::string exportString();
	NeuralNetwork(std::string importString);
	NeuralNetwork& operator=(const NeuralNetwork& nn){ inputSize = nn.inputSize; sizes = nn.sizes; learningRate = nn.learningRate; annealingRate = nn.annealingRate; nlayers = nn.nlayers; return *this; }
	size_t depth() const{ return nlayers.size(); };
	size_t getInputSize() const{ return inputSize; };
	size_t getOutputSize() const{ assert(sizes.size() > 0);  return sizes[sizes.size() - 1]; };
	AffineMatrix<double> operator[](size_t i) const{ return nlayers[i]; };
	
	void randInit();
	std::vector<double> frontprop(std::vector<double> input) const;

	//Genetic stuff
	NeuralNetwork mutate(double range) const;
	NeuralNetwork hybridize(const NeuralNetwork& n) const;

	//Backprop stuff
	std::vector<double> backprop(std::vector<double> testdata, std::vector<double> correct);
	std::vector<double> thresholding(std::vector<double> v) const;
	std::vector<double> thresholding_prime(std::vector<double> v) const;
};

std::ostream& operator<< (std::ostream& os, const NeuralNetwork& a);

