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
	bool getBool(vector<bool> prevLayer){
		double value = 0;
		//assert(prevLayer.size() == size && weights.size() - 1 == size);
		for (int i = 0; i < size; i++) if (prevLayer[i]) value += weights[i];
		return value > weights[size];
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
	vector<bool> getBools(vector<bool> prev){
		//clock_t c = clock();
		vector<bool> b;
		b.reserve(neurons.size());
		for (int i = 0; i < neurons.size(); i++)
			b.push_back(neurons[i].getBool(prev));
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
	int getScore(vector<vector<bool> > testdata, vector<bool> correct){
		int score = 0;
		for (int i = 0; i < testdata.size(); i++)//(Constant testdata between all things)
			if (correct[i] == getBools(testdata[i])[0]) score++; //Uses only the first neuron in the last layer.
		return score;
	}
	vector<bool> getBools(vector<bool> prev){
		for (int i = 0; i < nlayers.size(); i++)
			prev = nlayers[i].getBools(prev);
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

bool correctFunction(vector<bool>b){
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

	vector<NeuralNetwork> nets;
	vector<NeuralNetwork> newnets;
	for (int i = 0; i < netsPerGeneration; i++)
		nets.push_back(NeuralNetwork(inputSize,sizes));

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