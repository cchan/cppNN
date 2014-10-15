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


vector<vector<bool> > getTestdata(int tests, int testsize){
	static std::uniform_real_distribution<double> d(0.0, 1.0);

	vector<vector<bool> > t;
	vector<bool> b;
	for (int i = 0; i < tests; i++){
		for (int j = 0; j < testsize; j++) b.push_back(randDist(d)>0.5);
		t.push_back(b);
		b.clear();
	}
	return t;
}

template <class T> class Neuron{
public:
	int size;
	vector<double> weights;
	Neuron(){}
public:
	Neuron(int InputLayerSize){
		size = InputLayerSize;
		for (int i = 0; i < size+1; i++) weights.push_back(randStart());
	}
	T getOutput(vector<T> prevLayer){
		double value = 0;
		//assert(prevLayer.size() == size && weights.size() - 1 == size);
		for (int i = 0; i < size; i++) if (prevLayer[i]) value += weights[i];
		return value > weights[size];
	}
};
template <class T> class NeuronLayer{
public:
	vector<Neuron<T> > neurons;
	NeuronLayer(){}
public:
	NeuronLayer(int prevSize, int size){
		for (int i = 0; i < size; i++)
			neurons.push_back(Neuron<T>(prevSize));
	}
	vector<T> getOutput(vector<T> prev){
		//clock_t c = clock();
		vector<T> b;
		b.reserve(neurons.size());
		for (int i = 0; i < neurons.size(); i++)
			b.push_back(neurons[i].getOutput(prev));
		//cout << " (" << (clock() - c) / (CLOCKS_PER_SEC / 1000.0) << "ms) ";
		return b;
	}
};
template <class T> class NeuralNetwork{
private:
	int inputSize;
	vector<int> sizes;
	vector<NeuronLayer<T> > nlayers;
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
	T backprop(vector<T> testdatum, vector<T> correct){//Backpropagates deltas. Returns error, for idk purposes.
		int score = 0;
		vector<T> result = getOutput(testdatum);
		T diff = 0;
		for (int i = 0; i < correct.size(); i++){//"There can be multiple output neurons, in which case the error is the squared norm of the difference vector."
			T d = correct[i] - result[i];
			diff += d*d;
		}
		//so now diff is the "squared norm" error.


		/*
		Wikipedia:

		Backpropagation requires that the activation function used by the artificial neurons (or "nodes") be differentiable.
		[so does this apply to a bool neuron? <T> templates plz]


		Multiply its output delta and input activation to get the gradient of the weight.
		Subtract a ratio (percentage) of the gradient from the weight.
		*/
		for (int i = 0; i < correct.size(); i++)
			if (correct[i]==result[i]) score++;
			else {}//how do I backprop
		return score;
	}
	vector<T> getOutput(vector<T> prev){
		for (int i = 0; i < nlayers.size(); i++)
			prev = nlayers[i].getOutput(prev);
		return prev;
	}
};

double correctFunction(vector<double>b){
	return b[0];

	int sum = 0;
	for (int i = 0; i < b.size(); i++)if(b[i])sum++;
	return sum > b.size() / 2;
}



int main(){
	vector<int>sizes = { 10, 1 };
	int inputSize = 2;//How many bools in the vector<bool> tested on. Slightly proportional to overall time.
	int netsPerGeneration = 10;//Number of networks per generation. Proportional to overall time.
	int generations = 20;//Number of generations. Proportional to overall time.
	int tests = 20;//Number of tests applied to each member of a generation. Proportional to overall time.

	vector<vector<bool> > td;
	vector<bool> correct;

	cout << "Program execution begun. Initiating first generation..." << endl;

	vector<NeuralNetwork<double> > nets;
	vector<NeuralNetwork<double> > newnets;
	for (int i = 0; i < netsPerGeneration; i++)
		nets.push_back(NeuralNetwork<double>(inputSize,sizes));

	cout << "Initiated first generation. Beginning generations..." << endl;

	for (int n = 0; n < generations; n++){
		cout << "Testing generation " << n << "... ";
		td.clear();
		td = getTestdata(tests, inputSize);
		correct.clear();
		for (int i = 0; i < td.size(); i++)
			correct.push_back(correctFunction(td[i]));

		vector<int> scores;
		cout << "Scores (out of " << tests << "): ";
		for (int i = 0; i < netsPerGeneration; i++){
			scores.push_back(nets[i].getScore(td, correct));
			cout << " " << scores[i];
		}
		int best = 0;
		for (int i = 1; i < netsPerGeneration; i++)
			if (scores[i] > scores[best])
				best = i;
		cout << ". Mutating... ";

		//randMut(1.0 / (n+1));//It gets more and more precise. (Sets randMut plus/minus)
		newnets.push_back(nets[best]);//Keep the best.
		for (int i = 0; i < 9; i++)//Mutate the best.
			newnets.push_back(nets[best].getMutated());
		nets = newnets;
		newnets.clear();
		cout << "Done mutating." << endl;
	}
	system("pause");
}