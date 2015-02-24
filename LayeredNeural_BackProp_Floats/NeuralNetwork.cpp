#include "NeuralNetwork.h"

//Guaranteed: no parallelization

NeuralNetwork::NeuralNetwork(const size_t is, const std::vector<size_t>& s, double ar /* = 1.0*/){ //For some reason you can only define default val once, so it's defined in the *.h.
	learningRate = initialLearningRate; //Default; will be annealed.
	annealingRate = ar;
	inputSize = is;
	sizes = s;
	nlayers.reserve(sizes.size());
	for (size_t i = 0; i < sizes.size(); i++)
		nlayers.push_back(AffineMatrix<double>(size_t(sizes[i]), i == 0 ? inputSize : sizes[i - 1]));
}
/*
EXPORT FORMAT
[delim = " "]

BEGIN_NN_EXPORT
annealingRate
inputSize
numberOfLayers
[sizes]
[all nlayers, via callback]
END_NN_EXPORT
*/
std::string NeuralNetwork::exportString(){
	std::stringstream ss;
	ss << "BEGIN_NN_EXPORT " << annealingRate << " " << inputSize << " " << sizes.size() << " ";
	for (size_t size : sizes)ss << size << " ";

	for (AffineMatrix<double>& nl : nlayers)
		nl.callback([&ss](const size_t i, const size_t j, double& val){ ss << val << " "; }, [&ss](const size_t i, double& val){ ss << val << " "; });

	ss << "END_NN_EXPORT";

	return ss.str();
}
NeuralNetwork::NeuralNetwork(std::string importString){
	std::stringstream ss(importString);

	std::string begin_token, end_token;

	ss >> begin_token; //Indeed reads just one word at a time.
	assert(begin_token == "BEGIN_NN_EXPORT");

	learningRate = initialLearningRate;

	size_t NumOfLayers;
	assert(ss >> annealingRate >> inputSize >> NumOfLayers); // when ss has nothing left, it changes so that when cast to bool it becomes false - thus, detecting missings. - http://stackoverflow.com/a/12924210/1181387
	for (size_t i = 0; i < NumOfLayers; i++){
		int layerSize;
		assert(ss >> layerSize);
		sizes.push_back(layerSize);
	}

	//Init AffineMatrices
	nlayers.reserve(sizes.size());
	for (size_t i = 0; i < sizes.size(); i++)
		nlayers.push_back(AffineMatrix<double>(size_t(sizes[i]), i == 0 ? inputSize : sizes[i - 1]));
	//Input
	for (AffineMatrix<double>& nl : nlayers)
		nl.callback([&ss](const size_t i, const size_t j, double& val){ ss >> val; }, [&ss](const size_t i, double& val){ ss >> val; });
	//No assert()s while importing matrix values, for speed

	ss >> end_token; //No assert, since it might just be the last token, and then it'll be false once it's read out.
	assert(end_token == "END_NN_EXPORT");
}



void NeuralNetwork::randInit(){
	learningRate = initialLearningRate;
	for (AffineMatrix<double>& nl : nlayers)
		nl.callback([](){return rand11(); });
}
std::vector<double> NeuralNetwork::frontprop(std::vector<double> input) const{
	assert(input.size() == inputSize);
	input = nlayers[0] * input; //Skip first thresholding
	for (size_t i = 1; i < nlayers.size(); i++)
		input = (nlayers[i] * thresholding(input)); //Skip last thresholding
	return input;
}



NeuralNetwork NeuralNetwork::mutate(double range) const{ //range is used for SA
	NeuralNetwork thistmp(*this);

	//for each layer
	for (size_t i = 0; i < depth(); i++)
		thistmp.nlayers[i] = nlayers[i].mutate(range); //mutate

	return thistmp;
}
NeuralNetwork NeuralNetwork::hybridize(const NeuralNetwork& other) const{
	//uses random proportions of each to hybridize. Promotes genetic diversity I suppose.

	NeuralNetwork thistmp(*this);
	
	//for each layer
	assert(depth() == other.depth());
	for (size_t i = 0; i < depth(); i++)
		thistmp.nlayers[i] = nlayers[i].hybridize(other.nlayers[i]); //hybridize (will take care of verification of dimensions itself)

	return thistmp;
}


std::vector<double> NeuralNetwork::backprop(std::vector<double> testdata, std::vector<double> correct){
	//Based (loosely) on heavy reading from http://www.willamette.edu/~gorr/classes/cs449/backprop.html
	//And many hours of testing.

	assert(testdata.size() == inputSize);
	assert(correct.size() == getOutputSize());

	learningRate /= annealingRate; //Simulated Annealing, which is a fancy way of saying you make it change less as it learns more. Easy to come up with on your own :D

	AffineMatrix<double> matrixdelta;
	std::vector<std::vector<double> > activations; //"activations" means "output of previous layer"
	std::vector<double> delta;

	//Frontprop.
	activations.push_back(testdata);
	activations.push_back(nlayers[0] * testdata);
	for (size_t i = 1; i < nlayers.size(); i++)
		activations.push_back(nlayers[i] * thresholding(activations[i]));

	delta = thresholding(activations[activations.size() - 1]) - correct; //This last activation is only used for the delta.

	for (size_t i = nlayers.size() - 1; i < nlayers.size(); i--){ // < nlayers.size() because size_t (unsigned int) wraps back up to a very large integer.
		matrixdelta = affineOuterProduct(delta, activations[i]);
		delta = hadamardProduct(nlayers[i].transposeMultiply(delta), thresholding_prime(activations[i]));
		nlayers[i] -= learningRate * matrixdelta;
	}
	//std::cout << "correct: " << correct << "\nold: " << activations[activations.size() - 1] << "\nnew: " << frontprop(testdata) << "\n\n";
	return activations[activations.size() - 1];
}
std::vector<double> NeuralNetwork::thresholding(std::vector<double> v) const{
	std::for_each(v.begin(), v.end(), [](double& d){d = tanh(d); });
	return v;
}
std::vector<double> NeuralNetwork::thresholding_prime(std::vector<double> v) const{
	std::for_each(v.begin(), v.end(), [](double& d){d = 1 - tanh(d) * tanh(d); });
	return v;
}

std::ostream& operator<< (std::ostream& os, const NeuralNetwork& nn){
	for (size_t i = 0; i < nn.depth(); i++)
		os << "Layer " << i << ":" << std::endl << nn[i] << std::endl;
	return os;
}

