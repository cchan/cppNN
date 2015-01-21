#include <iostream>

#include "rand11.h"
#include "Vectors.h"
#include "NeuralNetwork.h"

using namespace std;

vector<double> correctFunction(vector<double> b, size_t outputSize);
vector<double> getTestdata(size_t inputSize);

int main(){
	const vector<size_t> sizes = { 5, 5 }; //Sizes of layers in the network.
	const size_t inputSize = 5;//How many doubles in the input vector. Slightly proportional to overall time.
	const size_t outputSize = sizes[sizes.size() - 1];

	const size_t generations = 500;
	const size_t netsPerGeneration = 8;
	const size_t testsPerGeneration = 1000;

	vector<NeuralNetwork> NNs(netsPerGeneration), newNNs(netsPerGeneration);
	vector<double> scores(netsPerGeneration,0), testdata, diff;

	for (size_t gen = 0; gen < generations; gen++){
		for (size_t i = 0; i < netsPerGeneration; i++){
			NNs[i] = NeuralNetwork(inputSize, sizes, 1);
			NNs[i].randInit();
		}

		for (size_t test = 0; test < testsPerGeneration; test++){
			testdata = getTestdata(inputSize);
			for (size_t i = 0; i < NNs.size(); i++){
				diff = correctFunction(testdata, outputSize) - NNs[i].frontprop(testdata);
				scores[i] += dotProduct(diff, diff);
			}
		}

		//Sort it by score from best to worst scores.
		vector<size_t> ranking(netsPerGeneration);
		for (size_t i = 0; i < netsPerGeneration; i++)ranking[i] = i;
		std::sort(ranking.begin(), ranking.end(), [scores](size_t a, size_t b){return scores[a] < scores[b]; });
		//cout << scores << ranking << endl;
		cout << "\nPrimary:" << NNs[ranking[0]][0] <<endl;

		for (size_t i = 0; i < netsPerGeneration; i++){ //Probabilistically
			//SHOULD DO IT BASED ON SCORE - pick two or three, and let the best of those mutate, then repeat
			//(isn't this a binomial distribution or something? oh it's poisson - it's tanks all over again! Max/min.)
			newNNs[i] = NNs[ranking[0]];// .mutate(0.00000001); //SA
			cout << newNNs[i][0] <<endl;
			scores[i] = 0;
		}

		NNs = newNNs;
	}

	/*
	//Backprop.

	const size_t backprops = 100000;//Number of backpropagations. Proportional to overall time. (REMEMBER TO SWITCH TO RELEASE CONFIG FOR ~20x SPEED!)
	const double annealingRate = 1.00001;//Different for different applications.

	cout << "Program execution begun." << endl;

	NeuralNetwork nn(inputSize, sizes, annealingRate);
	nn.randInit();

	vector<double> testdata;
	for (size_t n = 0; n < backprops; n++){
		testdata = getTestdata(inputSize);
		if (n % 1000 == 0)
			cout << correctFunction(testdata, outputSize) - nn.backprop(testdata, correctFunction(testdata, outputSize)) << endl;
		else
			nn.backprop(testdata, correctFunction(testdata, outputSize));
	}
	cout << "Done! Last delta was " << (correctFunction(testdata, outputSize) - nn.frontprop(testdata)) << ".\n\n\n";
	for (size_t i = 0; i < sizes.size(); i++)
		cout << nn[i] << endl;
	cin.get();
	*/
}


vector<double> correctFunction(vector<double> b, size_t outputSize){
	b.resize(0);
	b.resize(outputSize);
	return b;
}
vector<double> getTestdata(size_t inputSize){
	vector<double> b;
	b.reserve(inputSize);
	for (size_t i = 0; i < inputSize; i++) b.push_back(rand11());
	return b;
}
