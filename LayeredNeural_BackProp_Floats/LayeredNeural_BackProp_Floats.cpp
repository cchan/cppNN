#include <iostream>
#include <vector>
#include <random>

#include "NeuralNetwork.h"
#include "Vectors.h"

//Horrible-syntax convenience functions.
#define fori(x) for (size_t i = 0, fori_size = (x); i < fori_size; i++)
#define forj(x) for (size_t j = 0, forj_size = (x); j < forj_size; j++)

using namespace std;

double rand11();
vector<double> correctFunction(vector<double> b, size_t outputSize);
vector<double> getTestdata(size_t inputSize, double(*randfunc)());

int main(){
	const vector<size_t>sizes = { 3 }; //Sizes of layers in the network.
	const size_t inputSize = 5;//How many doubles in the input vector. Slightly proportional to overall time.
	const size_t backprops = 500;//Number of backpropagations. Proportional to overall time.
	const size_t outputSize = sizes[sizes.size() - 1];

	vector<double> testdata, delta, f;

	NeuralNetwork nn(inputSize, sizes, rand11);

	cout << "Program execution begun." << endl;

	testdata = getTestdata(inputSize, rand11);
	cout << "testdata:\n" << testdata << endl << endl;
	cout << "layer affinematrix:\n" << nn.nlayers[0] << endl << endl;
	cout << "sum of activations:\n" << (nn.nlayers[0] * testdata) << endl << endl;
	cout << "frontprop:\n" << nn.frontprop(testdata) << endl << endl;
	cout << "correct:\n" << correctFunction(testdata, outputSize) << endl << endl;
	cout << "delta:\n" << (nn.frontprop(testdata) - correctFunction(testdata, outputSize)) << endl << endl;
	cout << "outer product (activations, delta):\n" << outerProduct(nn.nlayers[0] * testdata, (nn.frontprop(testdata) - correctFunction(testdata, outputSize)));

	cin.get(); return 0;

	for (size_t n = 0; n < backprops; n++){
		testdata = getTestdata(inputSize, rand11);
		f = nn.frontprop(testdata);
		delta = f - correctFunction(testdata, outputSize);
		if (n % 100 == 0) cout << testdata << " => " << f << endl << endl;
		nn.backprop(delta);
	}
	cout << "Done! Best delta was " << testdata << " => " << f << "." << endl << endl << endl;
	for (AffineMatrix<double> a : nn.nlayers)
		cout << a << '[' << a.b() << ']' << endl << endl;
	cin.get();
}

double rand11(){
	static random_device rd;
	static mt19937 gen(rd());
	static uniform_real_distribution<double> d(-1.0, 1.0);
	return d(gen);
}
vector<double> correctFunction(vector<double> b, size_t outputSize){
	b.resize(1);
	b.resize(outputSize);
	return b;
}
vector<double> getTestdata(size_t inputSize, double(*randfunc)()){
	vector<double> b;
	b.reserve(inputSize);
	fori(inputSize) b.push_back(randfunc());
	return b;
}

