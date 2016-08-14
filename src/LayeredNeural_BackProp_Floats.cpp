#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <iomanip>

#include "common.h"
#include "Vectors.h"
#include "NeuralNetwork.h"

using namespace std;

double getScore(const NeuralNetwork& nn, vector<double> testdata);
vector<double> correctFunction(vector<double> b, size_t outputSize);
vector<double> getTestdata(size_t inputSize);
NeuralNetwork evolve(ostream& out, const size_t inputSize, const vector<size_t> sizes);
NeuralNetwork backprop(ostream& out, const size_t inputSize, const vector<size_t> sizes);

int main(){
	ofstream fout("C:/Users/Clive/Desktop/ScifairLogs/genetic.txt"); //Append?
	ofstream null;

	const vector<size_t> sizes = { 5, 5 }; //Sizes of layers in the network.
	const size_t inputSize = 5;//How many doubles in the input vector. Slightly proportional to overall time.
	
	std::time_t startTime = 0;
	NeuralNetwork bestNet;

	cout << startTimestamp(startTime)
		<< "NeuralNetwork inputSize: " << inputSize << endl
		<< "NeuralNetwork layer sizes: " << sizes << endl << endl;
	cout.precision(2);
	bestNet = backprop(cout, inputSize, sizes);
	cout << "\n\nFinal NN:\n" << bestNet << endl << endTimestamp(startTime);
	cin.get();
}

NeuralNetwork evolve(ostream& out, const size_t inputSize, const vector<size_t> sizes){
	const size_t outputSize = sizes[sizes.size() - 1];

	//Genetic.
	const size_t generations = 10000;
	const size_t netsPerGeneration = 20;
	const size_t testsPerGeneration = 500;
	const double annealingRateDecreaseRate = 1.003;
	double geneticAnnealingRate = 1.02;//Tradeoff: the smaller, the more accurate (less likely local minimum). The larger, the faster it converges. Just like actual annealing.
	double mutationRange = 0.2;
		//If AR too large, it starts giving the same NNs for each generation (the score variations between generations are because of testdata)
		//(could anneal the annealing rate too - the mutationrange consistently gets too small eventually)
		//Optimally, equals the rate of decrease of the score itself.
		//I think it has something to do with the lim n->inf rand(-1,1)^n average decrease rate (oh wait, isn't it like 1/2, since that's expected magnitude?)
		//I think it's the integral of 1/x, so it's ln |x|.
		//Note: best score and mutationRange should be approximately proportional throughout.
	/*const int fac = 20;
	for (int randiters = 0; randiters <= 20; randiters++){
		double squareddeviations = 0;
		vector<int> dist(2 * fac + 1);
		for (int i = 0; i < 100000; i++){
			double num = 1;
			for (int j = 0; j < randiters; j++)num *= rand11();
			dist[(int)round(num * fac) + fac]++;
			squareddeviations += num * num;
		}
		double stddev = sqrt(squareddeviations / 10000);
		double max = 0;
		for (int x : dist)
			if (x>max)max = x;
		for (int i = 0; i < dist.size(); i++){
			for (int j = 0; j < dist[i] * 79.0 / max; j++)
				cout << '-';
			cout << endl;
		}
		cout << randiters << ": " << stddev << endl;
		cin.get();
	}*/

	out << "GENETIC" << endl
		<< "Generations: " << generations << endl
		<< "NNs per generation: " << netsPerGeneration << endl
		<< "Number of tests: " << netsPerGeneration << endl
		<< "Annealing rate: " << geneticAnnealingRate << endl
		<< "Initial mutation range: " << mutationRange << endl << endl
		<< "Summed NN scores over all tests: " << endl;

	vector<NeuralNetwork> NNs(netsPerGeneration), newNNs(netsPerGeneration);
	vector<double> scores(netsPerGeneration, 0), testdata, diff;
	vector<size_t> ranking(netsPerGeneration);

	for (size_t i = 0; i < netsPerGeneration; i++){
		NNs[i] = NeuralNetwork(inputSize, sizes, 1);
		NNs[i].randInit();
	}

	for (size_t gen = 0; gen < generations; gen++){
		for (size_t test = 0; test <= testsPerGeneration; test++){
			testdata = getTestdata(inputSize);
			for (size_t i = 0; i < NNs.size(); i++)
				scores[i] += getScore(NNs[i], testdata);
		}

		//Sort it by score from best to worst scores.
		for (size_t i = 0; i < netsPerGeneration; i++)ranking[i] = i;
		std::sort(ranking.begin(), ranking.end(), [scores](size_t a, size_t b){return scores[a] < scores[b]; });

		if (gen % 10 == 0)out << "Generation " << gen << " best: " << scores[ranking[0]] << " dM: " << mutationRange << " SA: " << geneticAnnealingRate << endl;

		if (gen == generations)break;

		for (size_t netInGen = 0; netInGen < netsPerGeneration; netInGen++){ //Probabilistically
			//PROBABILISTICALLY BASED ON SCORE - pick four rankings at random, pick the best two of those (the lowest two numbers) and hybridize-mutate those rankings.
			const size_t pickHowMany = 6;
			vector<int> randIndex(netsPerGeneration);
			for (size_t i = 0; i < netsPerGeneration; i++) randIndex[i] = i;
			
			for (size_t i = 0; i < pickHowMany; i++) iter_swap(randIndex.begin() + i, randIndex.begin() + randInt(i, netsPerGeneration - 1));
			std::sort(randIndex.begin(), randIndex.begin() + pickHowMany); // + that many, because [first, last)

			newNNs[netInGen] = NNs[ranking[randIndex[0]]].hybridize(NNs[ranking[randIndex[1]]]).mutate(mutationRange); //SimulatedAnnealing
			scores[netInGen] = 0;
		}

		mutationRange /= geneticAnnealingRate;
		geneticAnnealingRate = 1 + (geneticAnnealingRate - 1) / annealingRateDecreaseRate;

		NNs = newNNs;

		//out << "NN[0]:" << endl << NNs[0] << endl << "Frontprop result: \n" << NNs[0].frontprop({ 1, 1, 1, 1, 1 }) << endl << endl
		//	<< "NN[1]:" << endl << NNs[1] << endl << "Frontprop result: \n" << NNs[1].frontprop({ 1, 1, 1, 1, 1 }) << endl << endl << endl << endl;
	}

	return NNs[ranking[0]];
}

NeuralNetwork backprop(ostream& out, const size_t inputSize, const vector<size_t> sizes){
	const size_t outputSize = sizes[sizes.size() - 1];

	//Backprop.
	const size_t displayperiod = 1000;//How often to display the result of the backprop (every * backprops)
	const size_t backprops = 100000;//Number of backpropagations. Proportional to overall time. (REMEMBER TO SWITCH TO RELEASE CONFIG FOR ~20x SPEED!)
	const double backpropAnnealingRate = 1.00001;//Different for different applications.

	out << "BACKPROP" << endl
		<< "Number of backpropagations: " << backprops << endl
		<< "Annealing Rate: " << backpropAnnealingRate << endl << endl
		<< "Deltas (every " << displayperiod << " backprops): " << endl;

	NeuralNetwork nn(inputSize, sizes, backpropAnnealingRate);
	nn.randInit();

	vector<double> testdata;
	for (size_t n = 0; n < backprops; n++){
		testdata = getTestdata(inputSize);
		if (n % displayperiod == 0)
			out << correctFunction(testdata, outputSize) - nn.backprop(testdata, correctFunction(testdata, outputSize)) << endl;
		else
			nn.backprop(testdata, correctFunction(testdata, outputSize));
	}
	out << "Done! Last delta was " << (correctFunction(testdata, outputSize) - nn.frontprop(testdata)) << ".\n\n";

	return nn;
}


double getScore(const NeuralNetwork& nn, vector<double> testdata){
	assert(nn.getInputSize() == testdata.size());
	vector<double> diff = correctFunction(testdata, nn.getOutputSize()) - nn.frontprop(testdata);
	return dotProduct(diff, diff);
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
