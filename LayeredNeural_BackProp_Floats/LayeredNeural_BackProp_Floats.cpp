#include <iostream>

#include "rand11.h"
#include "Vectors.h"
#include "NeuralNetwork.h"

//Horrible-syntax convenience functions.
#define fori(x) for (size_t i = 0, fori_size = (x); i < fori_size; i++)
#define forj(x) for (size_t j = 0, forj_size = (x); j < forj_size; j++)

using namespace std;

vector<double> correctFunction(vector<double> b, size_t outputSize);
vector<double> getTestdata(size_t inputSize);

int main(){
	const vector<size_t>sizes = { 5, 5 }; //Sizes of layers in the network.
	const size_t inputSize = 5;//How many doubles in the input vector. Slightly proportional to overall time.
	const size_t outputSize = sizes[sizes.size() - 1];

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
}


vector<double> correctFunction(vector<double> b, size_t outputSize){
	b.resize(0);
	b.resize(outputSize);
	return b;
}
vector<double> getTestdata(size_t inputSize){
	vector<double> b;
	b.reserve(inputSize);
	fori(inputSize) b.push_back(rand11());
	return b;
}

