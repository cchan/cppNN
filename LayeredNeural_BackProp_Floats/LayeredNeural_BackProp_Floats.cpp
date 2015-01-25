#include <iostream>
#include <ctime>
#include <string>
#include <fstream>

#include "rand11.h"
#include "Vectors.h"
#include "NeuralNetwork.h"

using namespace std;

double getScore(const NeuralNetwork& nn, vector<double> testdata);
vector<double> correctFunction(vector<double> b, size_t outputSize);
vector<double> getTestdata(size_t inputSize);
NeuralNetwork evolve(ostream& out, const size_t inputSize, const vector<size_t> sizes);
NeuralNetwork backprop(ostream& out, const size_t inputSize, const vector<size_t> sizes);

//http://en.cppreference.com/w/cpp/chrono/c/strftime
//https://stackoverflow.com/questions/14386923/localtime-vs-localtime-s-and-appropriate-input-arguments
string startTimestamp(time_t &startTime){
	string out = "";
	startTime = std::time(NULL);
	char timeFormatStr[100];
	struct tm timeinfo;
	localtime_s(&timeinfo, &startTime);
	if (std::strftime(timeFormatStr, sizeof(timeFormatStr), "%b %d, %Y %H:%M:%S", &timeinfo))
		out += "Started execution: " + (string)timeFormatStr + "\n\n";
	return out;
}
string endTimestamp(time_t startTime){
	string out = "";
	time_t endTime = std::time(NULL);
	char timeFormatStr[100];
	struct tm timeinfo;
	localtime_s(&timeinfo, &endTime);
	if (std::strftime(timeFormatStr, sizeof(timeFormatStr), "%b %d, %Y %H:%M:%S", &timeinfo))
		out += "\n\nCompleted execution: " + (string)timeFormatStr +"\n";
	out += "Total execution time: " + to_string(endTime - startTime) + " seconds\n";
	return out;
}

int main(){
	ofstream fout("C:/Users/Clive/Desktop/ScifairLogs/genetic.txt");
	ofstream null;

	const vector<size_t> sizes = { 5, 5 }; //Sizes of layers in the network.
	const size_t inputSize = 5;//How many doubles in the input vector. Slightly proportional to overall time.
	
	std::time_t startTime;
	fout << startTimestamp(startTime)
		<< "NeuralNetwork inputSize: " << inputSize << endl
		<< "NeuralNetwork layer sizes: " << sizes << endl << endl;
	NeuralNetwork bestNet = evolve(fout, inputSize, sizes);
	fout << endTimestamp(startTime);
	fout << "\nFinal NN:\n" << bestNet;
	cin.get();
}

NeuralNetwork evolve(ostream& out, const size_t inputSize, const vector<size_t> sizes){
	const size_t outputSize = sizes[sizes.size() - 1];

	//Genetic.
	const size_t generations = 3000;
	const size_t netsPerGeneration = 20;
	const size_t testsPerGeneration = 500;
	const double geneticAnnealingRate = 1.0001;//Tradeoff: the smaller, the more accurate (less likely local minimum). The larger, the faster it converges. Just like actual annealing.
		//If it's too large, it starts giving the same NNs for each generation (the score variations between generations are because of testdata)
	double mutationRange = 0.1;

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

		if (gen % 10 == 0)out << gen << ": " << scores << endl;

		if (gen == generations)return NNs[ranking[0]];

		for (size_t i = 0; i < netsPerGeneration; i++){ //Probabilistically
			//PROBABILISTICALLY BASED ON SCORE - pick four rankings at random, pick the best two of those (the lowest two numbers) and hybridize-mutate those rankings.
			const size_t pickHowMany = 6;
			vector<int> randIndex(netsPerGeneration);
			for (size_t i = 0; i < netsPerGeneration; i++) randIndex[i] = i;
			
			for (size_t i = 0; i < pickHowMany; i++) iter_swap(randIndex.begin() + i, randIndex.begin() + randInt(i, netsPerGeneration - 1));
			std::sort(randIndex.begin(), randIndex.begin() + pickHowMany); // + that many, because [first, last)

			newNNs[i] = NNs[ranking[randIndex[0]]].hybridize(NNs[ranking[randIndex[1]]]).mutate(mutationRange /= geneticAnnealingRate); //SimulatedAnnealing
			scores[i] = 0;
		}

		NNs = newNNs;

		//out << "NN[0]:" << endl << NNs[0] << endl << "Frontprop result: \n" << NNs[0].frontprop({ 1, 1, 1, 1, 1 }) << endl << endl
		//	<< "NN[1]:" << endl << NNs[1] << endl << "Frontprop result: \n" << NNs[1].frontprop({ 1, 1, 1, 1, 1 }) << endl << endl << endl << endl;
	}
}

NeuralNetwork backprop(ostream& out, const size_t inputSize, const vector<size_t> sizes){
	const size_t outputSize = sizes[sizes.size() - 1];

	//Backprop.
	const size_t backprops = 100000;//Number of backpropagations. Proportional to overall time. (REMEMBER TO SWITCH TO RELEASE CONFIG FOR ~20x SPEED!)
	const double backpropAnnealingRate = 1.00001;//Different for different applications.

	out << "BACKPROP" << endl
		<< "Number of backpropagations: " << backprops << endl
		<< "Annealing Rate: " << backpropAnnealingRate << endl << endl
		<< "Deltas: " << endl;

	NeuralNetwork nn(inputSize, sizes, backpropAnnealingRate);
	nn.randInit();

	vector<double> testdata;
	for (size_t n = 0; n < backprops; n++){
		testdata = getTestdata(inputSize);
		if (n % 1000 == 0)
			out << correctFunction(testdata, outputSize) - nn.backprop(testdata, correctFunction(testdata, outputSize)) << endl;
		else
			nn.backprop(testdata, correctFunction(testdata, outputSize));
	}
	out << "Done! Last delta was " << (correctFunction(testdata, outputSize) - nn.frontprop(testdata)) << ".\n\n\n";

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
