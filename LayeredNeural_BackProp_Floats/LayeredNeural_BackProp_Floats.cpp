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
	const vector<size_t>sizes = { 5, 5, 5, 5, 5 }; //Sizes of layers in the network.
	const size_t inputSize = 5;//How many doubles in the input vector. Slightly proportional to overall time.
	const size_t backprops = 5000;//Number of backpropagations. Proportional to overall time.
	const size_t outputSize = sizes[sizes.size() - 1];

	vector<double> testdata;
	vector<double> delta;

	NeuralNetwork nn(inputSize, sizes, rand11);

	cout << "Program execution begun." << endl;

	for (size_t n = 0; n < backprops; n++){
		//if(n%10==0)cout << n << endl;
		testdata = getTestdata(inputSize, rand11);
		vector<double> f = nn.frontprop(testdata);
		delta = - f + correctFunction(testdata, outputSize);
		if (n % 100 == 0) cout << testdata << " => " << f << endl << endl;
		nn.backprop(delta);
	}
	cout << "Done! Best delta was " << delta << "." << endl;
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
	double sum = 0;
	for (double d : b)sum += d;

	vector<double> o(outputSize, 0.0);
	o[0] = sum / 100.0;
	return o;
}
vector<double> getTestdata(size_t inputSize, double(*randfunc)()){
	vector<double> b;
	b.reserve(inputSize);
	fori(inputSize) b.push_back(randfunc());
	return b;
}

