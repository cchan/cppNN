#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(const size_t is, const std::vector<size_t>& s, double ar){
	learningRate = 0.5; //Default; will be annealed.
	annealingRate = ar;
	inputSize = is;
	sizes = s;
	nlayers.reserve(sizes.size());
	for (size_t i = 0; i < sizes.size(); i++)
		nlayers.push_back(AffineMatrix<double>(size_t(sizes[i]), i == 0 ? inputSize : sizes[i - 1]));
}
#include <iostream>
void NeuralNetwork::randInit(){
	for (AffineMatrix<double>& nl : nlayers)
		nl.callback([](size_t i, size_t j, double& c){c = rand11(); });
}
std::vector<double> NeuralNetwork::frontprop(std::vector<double> input){
	input = nlayers[0] * input; //Skip first thresholding
	for (size_t i = 1; i < nlayers.size(); i++)
		input = (nlayers[i] * thresholding(input)); //Skip last thresholding
	return input;
}



NeuralNetwork NeuralNetwork::getRandomlyMutated(double range) const{
	range * rand11();
}
NeuralNetwork NeuralNetwork::getRandomlyHybridized(const NeuralNetwork& other) const{
	//uses random proportions of each.
	double prop = rand11() / 2 + 1;
	//(prop * weight + (1-prop)*other.weight)
}



std::vector<double> NeuralNetwork::backprop(std::vector<double> testdata, std::vector<double> correct){
	//Based on heavy reading from http://www.willamette.edu/~gorr/classes/cs449/backprop.html
	//And many hours of testing.

	learningRate /= annealingRate; //Simulated Annealing, which is a fancy way of saying you make it change less as it learns more. Easy to come up with on your own :D

	AffineMatrix<double> matrixdelta;
	std::vector<std::vector<double> > activations;
	std::vector<double> delta;

	//Frontprop.
	activations.push_back(testdata);
	activations.push_back(nlayers[0] * testdata);
	for (int i = 1; i < nlayers.size(); i++)
		activations.push_back(nlayers[i] * thresholding(activations[i]));

	delta = thresholding(activations[activations.size() - 1]) - correct; //This last activation is only used for the delta.

	for (int i = nlayers.size() - 1; i >= 0; i--){
		matrixdelta = affineOuterProduct(delta, activations[i]);
		delta = hadamardProduct(nlayers[i].transposeMultiply(delta), thresholding_prime(activations[i]));
		nlayers[i] -= learningRate * matrixdelta;
	}
	//std::cout << "correct: " << correct << "\nold: " << activations[activations.size() - 1] << "\nnew: " << frontprop(testdata) << "\n\n";
	return activations[activations.size() - 1];
}
std::vector<double> NeuralNetwork::thresholding(std::vector<double> v){
	std::for_each(v.begin(), v.end(), [](double& d){d = tanh(d); });
	return v;
}
std::vector<double> NeuralNetwork::thresholding_prime(std::vector<double> v){
	std::for_each(v.begin(), v.end(), [](double& d){d = 1 - tanh(d) * tanh(d); });
	return v;
}

