#include <iostream>
#include <vector>
#include <algorithm>
//#define NDEBUG
#include <cassert>
#include <random>
#include <ctime>

using namespace std;

double randDist(std::uniform_real_distribution<double> d){
	static std::random_device rd;
	static std::mt19937 gen(rd());
	double p = d(gen);
	//cout << "(("<< p <<"))";
	return p;
}
double randStart(){
	static std::uniform_real_distribution<double> d(-1.0, 1.0);
	return randDist(d);
}
double randMut(double r = -1.0){
	static double range = 1.0;
	if (r > 0.0 && r <= 1.0)range = r;
	static std::uniform_real_distribution<double> d(-range, range);
	return randDist(d);
}


vector<vector<double> > getTestdata(int tests, int testsize){
	static std::uniform_real_distribution<double> d(0.0, 1.0);

	vector<vector<double> > t;
	vector<double> b;
	for (int i = 0; i < tests; i++){
		for (int j = 0; j < testsize; j++) b.push_back(randDist(d));
		t.push_back(b);
		b.clear();
	}
	return t;
}

class Neuron{
public:
	int size;
	vector<double> weights;
	Neuron(){}
public:
	Neuron(int InputLayerSize){
		size = InputLayerSize;
		for (int i = 0; i < size+1; i++) weights.push_back(randStart());
	}
	double getVal(vector<double> prevLayer){
		double value = 0;
		//assert(prevLayer.size() == size && weights.size() - 1 == size);
		for (int i = 0; i < size; i++) if (prevLayer[i]) value += prevLayer[i] * weights[i];
		return thresholding(value);
	}
	/*
	               __________
	              /
	     ________/
	This could be an easy thresholding function - constant, except linear in the middle.
	*/
	double thresholding(value){
		double threshold = weights[size];//THRESHOLD to compare to.
		double width = 0.5;//WIDTH (radius) of that slant in the middle
		double bottom = -1;//lower value
		double top = +1;//upper value
		
		if(threshold - width > value) return bottom;
		else if(threshold + width < value) return top;
		else return bottom + ((top - bottom)/2) + ((top - bottom)/2) * (value - threshold) / width; //test: value=threshold+width...
	}
	Neuron getMutated(){
		Neuron n;
		n.size = size;
		n.weights.reserve(size + 1);
		for (int i = 0; i < size + 1; i++)
			n.weights.push_back(weights[i] + randMut());
		return n;
	}
};
class NeuronLayer{
public:
	vector<Neuron> neurons;
	NeuronLayer(){}
public:
	NeuronLayer(int prevSize, int size){
		for (int i = 0; i < size; i++)
			neurons.push_back(Neuron(prevSize));
	}
	vector<double> getVals(vector<double> prev){
		//clock_t c = clock();
		vector<double> b;
		b.reserve(neurons.size());
		for (int i = 0; i < neurons.size(); i++)
			b.push_back(neurons[i].getVal(prev));
		//cout << " (" << (clock() - c) / (CLOCKS_PER_SEC / 1000.0) << "ms) ";
		return b;
	}
	NeuronLayer getMutated(){
		NeuronLayer n;
		for (int i = 0; i < neurons.size(); i++)
			n.neurons.push_back(neurons[i].getMutated());
		return n;
	}
};
class NeuralNetwork{
private:
	int inputSize;
	vector<int> sizes;
	vector<NeuronLayer> nlayers;
	NeuralNetwork(int is, vector<int> s, vector<NeuronLayer> n){
		inputSize = is;
		sizes = s;
		nlayers = n;
	}
public:
	NeuralNetwork(int is, vector<int> s){
		inputSize = is;
		sizes = s;
		for (int i = 0; i < sizes.size(); i++)
			nlayers.push_back(NeuronLayer(i==0? inputSize : sizes[i-1], sizes[i]));
	}
	int getScore(vector<vector<double> > testdata, vector<double> correct){
		int score = 0;
		for (int i = 0; i < testdata.size(); i++)//(Constant testdata between all things)
			if (correct[i] == getVals(testdata[i])[0]) score++; //Uses only the first neuron in the last layer.
		return score;
	}
	vector<double> getVals(vector<double> prev){
		for (int i = 0; i < nlayers.size(); i++)
			prev = nlayers[i].getVals(prev);
		return prev;
	}
	NeuralNetwork getMutated(){
		vector<NeuronLayer> n;
		for (int i = 0; i < nlayers.size(); i++)
			n.push_back(nlayers[i].getMutated());
		cout << n[0].neurons[0].weights[0] << " ";
		return NeuralNetwork(inputSize, sizes, n);
	}
};

vector<double> correctFunction(vector<double>b, int size){
	vector<double> r;
	for (int i = 0; i<size(); i++) if(i >= b.size())r.push_back(0); else r.push_back(b[i]);
	return r;
	
	/*int sum = 0;
	for (int i = 0; i < b.size(); i++)if(b[i])sum++;
	return sum > b.size() / 2;*/
}



int main(){
	vector<int>sizes = { 10, 10 };
	int inputSize = 2;//How many doubles in the input double vector tested on. Slightly proportional to overall time.
	int netsPerGeneration = 10;//Number of networks per generation. Proportional to overall time.
	int generations = 20;//Number of generations. Proportional to overall time.
	int tests = 20;//Number of tests applied to each member of a generation. Proportional to overall time.

	vector<vector<double> > testdata;
	vector<vector<double> > correct;

	cout << "Program execution begun. Initiating first generation..." << endl;

	vector<NeuralNetwork> nets;
	vector<NeuralNetwork> newnets;
	for (int i = 0; i < netsPerGeneration; i++)
		nets.push_back(NeuralNetwork(inputSize,sizes));

	cout << "Initiated first generation. Beginning generations..." << endl;

	for (int n = 0; n < generations; n++){
		cout << "Testing generation " << n << "... ";
		testdata.clear();
		testdata = getTestdata(tests, inputSize);
		correct.clear();
		for (int i = 0; i < testdata.size(); i++)
			correct.push_back(correctFunction(testdata[i]));

		vector<int> scores;
		cout << "Scores (out of " << tests << "): ";
		for (int i = 0; i < netsPerGeneration; i++){
			scores.push_back(nets[i].getScore(testdata, correct));
			cout << " " << scores[i];
		}
		int best = 0;
		for (int i = 1; i < netsPerGeneration; i++)
			if (scores[i] > scores[best])
				best = i;
		cout << ". Mutating... ";

		randMut(1.0 / (n+1));//It gets more and more precise. (Sets randMut plus/minus)
		newnets.push_back(nets[best]);//Keep the best.
		for (int i = 0; i < 9; i++)//Mutate the best.
			newnets.push_back(nets[best].getMutated());
		nets = newnets;
		newnets.clear();
		cout << "Done mutating." << endl;
	}
	system("pause");
}