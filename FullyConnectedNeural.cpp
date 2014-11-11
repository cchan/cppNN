#include <iostream>
#include <vector>
#include <algorithm>
//#define NDEBUG
#include <cassert>
#include <random>
#include <ctime>

//Ideas:
//Holy s**t this is cool: https://www.youtube.com/watch?v=n1ViNeWhC24
//class randDbl - make value-getting somewhat random.

//Horrible-syntax convenience functions.
#define fori(x) for (int i = 0, fori_size = (x); i < fori_size; i++)
#define forj(x) for (int j = 0, forj_size = (x); j < forj_size; j++)

using namespace std;

inline double randDist(std::uniform_real_distribution<double> d){
	static std::random_device rd;
	static std::mt19937 gen(rd());
	double p = d(gen);
	//cout << "(("<< p <<"))";
	return p;
}
inline double randStart(){
	static std::uniform_real_distribution<double> d(-1.0, 1.0);
	return randDist(d);
}
inline double randMut(double r = -1.0){
	static double range = 1.0;
	if (r > 0.0 && r <= 1.0)range = r;
	static std::uniform_real_distribution<double> d(-range, range);
	return randDist(d);
}


double squaredDistance(const vector<double>& a, const vector<double>& b){
	double sum = 0;
	fori(a.size())
		sum += (a[i] - b[i]) * (a[i] - b[i]);
	return sum;
}


vector<vector<double> > getTestdata(int tests, int testsize){
	static std::uniform_real_distribution<double> d(0.0, 1.0);

	vector<vector<double> > t;
	vector<double> b;
	t.reserve(tests);
	b.reserve(testsize);
	fori(tests){
		forj(testsize) b.push_back(randDist(d));
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
	Neuron(int inputLayerSize){
		size = inputLayerSize;
		fori(size+1) weights.push_back(randStart());
	}
	double getVal(const vector<double>& prevLayer){
		double value = 0;
		//assert(prevLayer.size() == size && weights.size() - 1 == size);
		fori(size) if (prevLayer[i]) value += prevLayer[i] * weights[i];
		return thresholding(value);
	}
	/*
	               __________
	              /
	     ________/
	This could be an easy thresholding function - constant, except linear in the middle.
	*/
	inline double thresholding(double value){
		return atan(value/10)*2/3.141592653589; //Between -pi/2 and +pi/2 degrees.
	}
	Neuron getMutated(){
		Neuron n;
		n.size = size;
		n.weights.reserve(size + 1);
		fori(size+1)
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
		fori(size)
			neurons.push_back(Neuron(prevSize));
	}
	vector<double> getVals(const vector<double>& prev){
		//clock_t c = clock();
		vector<double> b;
		b.reserve(neurons.size());
		fori(neurons.size())
			b.push_back(neurons[i].getVal(prev));
		//cout << " (" << (clock() - c) / (CLOCKS_PER_SEC / 1000.0) << "ms) ";
		return b;
	}
	NeuronLayer getMutated(){
		NeuronLayer n;
		fori(neurons.size())
			n.neurons.push_back(neurons[i].getMutated());
		return n;
	}
};
class NeuralNetwork{
private:
	int inputSize;
	vector<int> sizes;
	vector<NeuronLayer> nlayers;
	vector<double> previousInput;
	vector<double> previousOutput;
	NeuralNetwork(){}
public:
	NeuralNetwork(int is, const vector<int>& s){
		inputSize = is;
		sizes = s;
		fori(sizes.size())
			nlayers.push_back(NeuronLayer(i==0? inputSize : sizes[i-1], sizes[i]));
	}
	vector<double> feedback(double score){

	}
	vector<double> getVals(vector<double> input){
		fori(nlayers.size())
			input = nlayers[i].getVals(input);
		previousInput = input;
		return input;
	}
};


vector<double> correctFunction(vector<double> b, int outputSize){
	b.clear();
	b.resize(outputSize,0.5);
	return b;
}



int main(){
	const vector<int>sizes = { 5, 5, 5, 5, 5};
	const int inputSize = 5;//How many doubles in the input vector. Slightly proportional to overall time.
	const int netsPerGeneration = 10;//Number of networks per generation. Proportional to overall time.
	const int generations = 100;//Number of generations. Proportional to overall time.
	const int tests = 200;//Number of tests applied to each member of a generation. Proportional to overall time.
	const int outputSize = sizes[sizes.size() - 1];

	vector<vector<double> > testdata;
	vector<vector<double> > correct;
	vector<double> scores;
	int best;

	cout << "Program execution begun. Initiating first generation..." << endl;

	vector<NeuralNetwork> nets;
	fori(netsPerGeneration)
		nets.push_back(NeuralNetwork(inputSize,sizes));
	
	cout << "Initiated first generation. Beginning generations..." << endl;

	for (int n = 0; n < generations; n++){
		cout << "Gen" << n << "... ";
		
		testdata = getTestdata(tests, inputSize);
		correct.clear();
		fori(testdata.size())
			correct.push_back(correctFunction(testdata[i],sizes[sizes.size()-1]));

		scores.clear();
		//cout << "Scores: ";
		fori(netsPerGeneration){
			scores.push_back(nets[i].getScore(testdata, correct));
			//cout << " " << scores[i];
		}
		best = 0;
		//secondbest = 1;
		fori(netsPerGeneration)
			if (scores[i] < scores[best])
				best = i;
		//cout << ". ";

		if (n < generations - 1){
			cout << "Best = " << scores[best] << ". Mutating... ";

			//randMut(1.0 / (n + 1));//It gets more and more precise. (Sets randMut plus/minus)

			fori(netsPerGeneration)//Mutate the best and fill the rest of the spots with it.
				if (i != best)nets[i] = nets[best].getMutated();

			cout << "Done." << endl;
		}
	}
	cout << "Done! Best score was " << scores[best] << ". Press any key to continue.";
	cin.get();
}
