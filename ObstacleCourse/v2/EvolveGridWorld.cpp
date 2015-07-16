#include <iostream>
#include <fstream>
#include <ppl.h> //http://www.drdobbs.com/parallel/how-to-write-a-parallelfor-loop-matrix-m/228800433
#include "GridWorld.h"

using namespace std;

/*
//Issues:
//NeuralNetwork:
	//AffineMatrix is dumb.
	//Range should be [0,1) for all things
	//Just do all of the above in NNArmadillo
//GridWorld:
	//Need to timestep them all simultaneously.
	//Looking for advanced features? unsupervisedness

//Don't remove robots (# of positions input into NN needs to have const size), just add points for proximity - live point score //but then how do they run away? need doublespeed with cooldown
	//Or just respawn
*/

int main(){

	/*

	if (false){
	cin.get();
	replay(cout, "", "", 50, 20);
	setConsoleXY(0, 0);
	cin.get();
	replay(cout, "", "", 50, 20);
	cin.get();
	return 0;
	}

	*/


	//Config. TODO: make it more configurable.
	resizeWindow(800, 500);
	system("color f0");
	double mutationRange = 0.4;
	double annealingRate = 1.000;
	const int steps_per_battle = 100;
	int XHowMany = 2, OHowMany = 4;

	vector<pair<NeuralNetwork, pair<int, int> > > nnX(5), nnO(5);

	//Initialize sizes and randomness.
	for (auto &nn : nnX){
		nn.first = NeuralNetwork((XHowMany + OHowMany) * 2 + 2, { 10, 10, 2 });
		nn.first.randInit();
	}
	for (auto &nn : nnO){
		nn.first = NeuralNetwork((XHowMany + OHowMany) * 2 + 2, { 10, 10, 2 });
		nn.first.randInit();
	}

	ofstream NN_fout("C:/Users/Clive/Desktop/github/scifair/ObstacleCourse/NN.export.txt", fstream::out | fstream::app);
	ofstream Scores_fout("C:/Users/Clive/Desktop/github/scifair/ObstacleCourse/Scores.export.txt", fstream::out | fstream::app);
	std::time_t startTime_NN = 0, startTime_Scores = 0;
	NN_fout << endl << endl << startTimestamp(startTime_NN);
	Scores_fout << endl << endl << startTimestamp(startTime_Scores);

	for (int generation = 0; generation < 100000; generation++){
		setConsoleXY(0, 0);
		cout << "Gen: " << generation << endl;
		concurrency::parallel_for(size_t(0), nnX.size()*nnO.size(), [&](size_t iXO){//the parallelized tasks should be smaller... parallel_for over the steps?
			int iX = iXO % nnX.size();
			int iO = iXO / nnX.size();
			Battle b(XHowMany, OHowMany, nnX[iX].first, nnO[iO].first);
			if (iXO == 0){//Output the first match of every generation (nnO[0] vs nnX[0])
				cout << (char)0xda; for (int i = 0; i < Battle::fieldsize; i++){ cout << (char)0xc4; } cout << (char)0xbf << endl;
				COORD coord = getConsoleCoord();
				for (int i = 0; i < steps_per_battle && b.ORobots.size() > 0; i++){
					b.step();
					if (i % 2 == 0){
						b.output(cout);
						setConsoleCoord(coord);
						/*cout << b.getCurrentOScore(); cin.get();*/
					}
				}
				b.output(cout);
				cout << (char)0xc0; for (int i = 0; i < Battle::fieldsize; i++){ cout << (char)0xc4; } cout << (char)0xd9 << endl;
				cout << endl
					<< "Done displaying " << generation << ":" << endl
					<< "  Steps per battle: " << steps_per_battle << endl
					<< "  OLifetimeSum: " << b.getCurrentOScore() << "    " << endl
					<< "  XTraveled: " << b.getXTraveled() << "    " << endl
					<< "  OTraveled: " << b.getOTraveled() << "    " << endl;
			}
			else{
				for (int i = 0; i < steps_per_battle && b.ORobots.size() > 0; i++)
					b.step();
			}
			nnX[iX].second.first += b.getCurrentOScore();
			nnO[iO].second.first += b.getCurrentOScore();
			nnX[iX].second.second += b.getXTraveled();
			nnO[iO].second.second += b.getOTraveled();
		});

		NN_fout << generation << " X: " << nnX[0].first.exportString() << endl
			<< generation << " O: " << nnO[0].first.exportString() << endl;

		cout << "Scored all battles, gen " << generation << ".\nMutate " << mutationRange << endl << "[SA " << annealingRate << "]";

		//For X, you're trying to minimize the O lifetime, or to tiebreak maximize the disttraveled.
		sort(nnX.begin(), nnX.end(), [](pair<NeuralNetwork, pair<int, int>> n1, pair<NeuralNetwork, pair<int, int>> n2){if (n1.second.first == n2.second.first) return n1.second.second > n2.second.second; return n1.second.first < n2.second.first; });

		NeuralNetwork tmpX = nnX[0].first, tmpX2 = nnX[1].first;
		for (int i = 0; i < nnX.size() / 2; i++){
			nnX[i].first = tmpX.mutate(mutationRange /= annealingRate);
			nnX[i].second.first = nnX[i].second.second = 0;
		}
		for (int i = nnX.size() / 2; i < nnX.size(); i++){
			nnX[i].first = tmpX.hybridize(tmpX2);
			nnX[i].second.first = nnX[i].second.second = 0;
		}


		//For O, you're trying to maximize the O lifetime, or to tiebreak maximize the distTraveled
		sort(nnO.begin(), nnO.end(), [](pair<NeuralNetwork, pair<int, int>> n1, pair<NeuralNetwork, pair<int, int>> n2){if (n1.second.first == n2.second.first) return n1.second.second > n2.second.second; return n1.second.first > n2.second.first; });

		Scores_fout << nnO[0].second.first << " " << flush;

		NeuralNetwork tmpO = nnO[0].first, tmpO2 = nnO[1].first;
		for (int i = 0; i < nnO.size() / 2; i++){
			nnO[i].first = tmpO.mutate(mutationRange /= annealingRate);
			nnO[i].second.first = nnO[i].second.second = 0;
		}
		for (int i = nnO.size() / 2; i < nnO.size(); i++){
			nnO[i].first = tmpO.hybridize(tmpO2).mutate(mutationRange /= annealingRate);
			nnO[i].second.first = nnO[i].second.second = 0;
		}
	}

	NN_fout << endTimestamp(startTime_NN);
	Scores_fout << endTimestamp(startTime_Scores);

	cin.get();
	return 0;
}
