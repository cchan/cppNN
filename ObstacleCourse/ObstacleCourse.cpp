#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <cassert>
#include "../common.h"
#include "../LayeredNeural_Backprop_Floats/NeuralNetwork.h"
#include "time.h"

//#include <thread> //http://www.cplusplus.com/reference/thread/thread/

#include <ppl.h> //http://www.drdobbs.com/parallel/how-to-write-a-parallelfor-loop-matrix-m/228800433
using namespace concurrency;
using namespace std;

#define _WIN32_WINNT 0x0500
#include <windows.h>

//http://www.cplusplus.com/forum/beginner/1481/
void resizeWindow(int xpx, int ypx){
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions

	//MoveWindow(window_handle, x, y, width, height, redraw_window);
	MoveWindow(console, r.left, r.top, xpx, ypx, TRUE);
}

//http://stackoverflow.com/a/22685071/1181387
void gotoConsoleXY(int column, int line)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
		);
}


class Robot{
public:
	NeuralNetwork& brain;
	int x, y;
	int FOVMax, FOVMin;

	//double collabValue; //Outputs a value which is inputted into all other NNs of its type, so it can sort of "collaborate". Sequential vs. layered updating will be awkward.

	Robot(NeuralNetwork& _brain, int _x, int _y, int _team, int _FOVMin, int _FOVMax) : brain(_brain){
		x = _x;
		y = _y;
		FOVMin = _FOVMin;
		FOVMax = _FOVMax;
	}
	Robot(const Robot& other) : brain(other.brain){
		x = other.x;
		y = other.y;
		FOVMin = other.FOVMin;
		FOVMax = other.FOVMax;
	}
	Robot operator=(const Robot& other){
		brain = other.brain;
		x = other.x;
		y = other.y;
		FOVMin = other.FOVMin;
		FOVMax = other.FOVMax;
		return other;
	}

	int step(const vector<vector<int> >& field, int stepnum){ //Field: +1 if X team, -1 if O team, 0 if nothing. Same width and height.
		assert(brain.getOutputSize() == 2); //Because step() wouldn't work otherwise.
		assert(brain.getInputSize() == (FOVMax - FOVMin + 1)*(FOVMax - FOVMin + 1) + 3); //Make sure it's the right inputsize too. 

		vector<double> proximity;
		proximity.reserve((FOVMax - FOVMin + 1)*(FOVMax - FOVMin + 1) + 3);
		for (int i = FOVMin; i <= FOVMax; i++){
			for (int j = FOVMin; j <= FOVMax; j++){
				int readX = x + i;
				int readY = y + j;
				if (readX >= field.size() || readX < 0 || readY >= field.size() || readY < 0) //Off-field
					proximity.push_back(0);//2
				else
					proximity.push_back(field[readX][readY]);
			}
		}

		//Three more pieces of info. Might come in handy.
		proximity.push_back(x);
		proximity.push_back(y);
		proximity.push_back(stepnum); //Basically, what time is it

		vector<double> newpos = brain.thresholding(brain.frontprop(proximity));

		//Moves itself based on that brain's response.
		// (1,1)  (1,0)  (1,-1)
		// (0,1)  (0,0)  (0,-1)
		// (-1,1) (-1,0) (-1,-1)
		int retval = 0;
		if (newpos[0] > 0.3333333 && x < field.size() - 1 && field[x + 1][y] == 0) { retval++; x++; }//if it's in the correct range, and it's still on the field, and it's not occupied
		else if (newpos[0] < -0.3333333 && x > 0 && field[x - 1][y] == 0) { retval++; x--; }
		if (newpos[1] > 0.3333333  && y < field.size() - 1 && field[x][y + 1] == 0) { retval++; y++; }
		else if (newpos[1] < -0.3333333 && y > 0 && field[x][y - 1] == 0) { retval++; y--; }
		return retval; //Taxicab distance.
	}
};

class XRobot : public Robot{
public:
	XRobot(NeuralNetwork& _brain, int _x, int _y, int _team, int _FOVMin, int _FOVMax) : Robot(_brain, _x, _y, _team, _FOVMin, _FOVMax){}
};

class ORobot : public Robot{
public:
	ORobot(NeuralNetwork& _brain, int _x, int _y, int _team, int _FOVMin, int _FOVMax) : Robot(_brain, _x, _y, _team, _FOVMin, _FOVMax){}
	bool isEaten(const vector<XRobot>& XRobots) const{ //Within 1, in both X and Y coords.
		for (XRobot r : XRobots)
			if (abs(x - r.x) <= 1 && abs(y - r.y) <= 1)
				return true;
		return false;
	}
};

class Battle{
public:
	const static int fieldsize = 20;
	const int startingX = 2, startingO = 4;
	const int FOVMax = 4, FOVMin = -4;
	int OScore = 0, OTraveledScore = 0, XTraveledScore = 0, stepnum = 0;
	vector<int> OLifetimes;
	vector<vector<int> > field;

	vector<XRobot> XRobots;
	vector<ORobot> ORobots;

	Battle(NeuralNetwork& nnX, NeuralNetwork& nnO){
		int x, y;
		for (int i = 0; i < startingX; i++){
			x = fieldsize / 4;
			y = fieldsize / (startingX + 1) * (1 + i);

			XRobots.push_back(XRobot(nnX, x, y, 1, FOVMin, FOVMax));
		}
		for (int i = 0; i < startingO; i++){
			x = 3 * fieldsize / 4;
			y = fieldsize / (startingO + 1) * (1 + i);

			ORobots.push_back(ORobot(nnO, x, y, -1, FOVMin, FOVMax));
		}

		OLifetimes = vector<int>(ORobots.size(), 0);

		field = getField();
	}
	vector<vector<int> > getField(){
		vector<vector<int> > field(fieldsize);
		for (auto &v : field)v = vector<int>(fieldsize);//Init to zero
		for (const Robot &r : XRobots)field[r.x][r.y] = 1;//Add XRobots
		for (const Robot &r : ORobots)field[r.x][r.y] = -1;//Add ORobots
		return field;
	}
	int getCurrentOScore(){
		int currOScore = OScore;
		for (int i = 0; i < ORobots.size(); i++)
			currOScore += OLifetimes[i];
		return currOScore;
	}
	int getOTraveled(){
		return OTraveledScore;
	}
	int getXTraveled(){
		return XTraveledScore;
	}
	void step(){
		//Eating
		for (int i = ORobots.size() - 1; i >= 0; i--){//Backwards, so that the downshifting when deleted doesn't affect anything.
			if (ORobots[i].isEaten(XRobots)){
				field[ORobots[i].x][ORobots[i].y] = 0;
				OScore += OLifetimes[i];
				ORobots.erase(ORobots.begin() + i);
				OLifetimes.erase(OLifetimes.begin() + i);
			}
		}

		//Moving XRobots
		for (XRobot &r : XRobots){
			int oldx = r.x, oldy = r.y;
			XTraveledScore += r.step(field, stepnum);
			field[oldx][oldy] = 0; field[r.x][r.y] = 1;
		}
		
		//Moving ORobots
		for (int i = 0; i < ORobots.size(); i++){
			int oldx = ORobots[i].x, oldy = ORobots[i].y;
			OTraveledScore += ORobots[i].step(field, stepnum);
			OLifetimes[i]++;
			field[oldx][oldy] = 0; field[ORobots[i].x][ORobots[i].y] = -1;
		}

		stepnum++;
	}
	void output(){//Outputs the Xs, Os, and uses '.' for anything in the field of view.
		gotoConsoleXY(0, 2);
		for (const XRobot &r : XRobots){
			for (int dx = max(FOVMin + r.x, 0); dx <= min(FOVMax + r.x, fieldsize - 1); dx++)
				for (int dy = max(FOVMin + r.y, 0); dy <= min(FOVMax + r.y, fieldsize - 1); dy++)
					if (field[dx][dy] == 0)
						field[dx][dy] = 2;
		}
		for (int i = 0; i < fieldsize; i++){
			cout << (char)0xb3;
			for (int j = 0; j < fieldsize; j++){
				if (field[i][j] == 0)cout << ' ';
				else if (field[i][j] == 2){ cout << (char)0xfa; field[i][j] = 0; } //OEM extended ascii
				else if (field[i][j] == -1)cout << 'O';
				else if (field[i][j] == 1)cout << 'X';
				else { cout << "Invalid value: " << field[i][j]; assert(false); }
			}
			cout << (char)0xb3 << endl;
		}
	}
};



int main(){
	/*
	Game: X Team is trying to "eat" O Team robots. All robots on a team have the same base NN.
		There are 5 of each on the 20x20 field in the beginning. (May want to balance numbers, predator/prey, but not necessary)
		If a X Team robot becomes adjacent to an O Team robot at the end of any timestep, that O Team robot disappears.
	Generations
		The teams will evolve separately; the one achieving highest overall score against all of the enemy teams wins.
		"Score" will be determined by sum of the lifetimes of the Os (to a max of some number of steps), summed across all battles.
			XRobot will be sorted in order, ORobot will be sorted in reverse order.
		Mutation will be as usual. Annealing probably is necessary.
	NeuralNetwork
		Input: the n^2 cells surrounding/under each robot:
			0 if nothing
			1 if X team
			-1 if O team
		Output: which of the 9 cells surrounding/under the robot it should move to.
	*/


	//Config. TODO: make it more configurable.
	resizeWindow(800, 500);
	double mutationRange = 0.4;
	double annealingRate = 1.000;
	const int steps_per_battle = 100;
	
	vector<pair<NeuralNetwork, pair<int, int> > > nnX(5), nnO(5);

	//Initialize sizes and randomness.
	for (auto &nn : nnX){
		nn.first = NeuralNetwork(84, { 10, 2 });
		nn.first.randInit();
	}
	for (auto &nn : nnO){
		nn.first = NeuralNetwork(84, { 10, 2 });
		nn.first.randInit();
	}

	ofstream fout("C:/Users/Clive/Desktop/testNNExport.txt", fstream::out | fstream::app);
	std::time_t startTime = 0;
	fout << startTimestamp(startTime);

	for (int generation = 0; generation < 100000; generation++){
		gotoConsoleXY(0, 0);
		cout << "Gen: " << generation << endl;
		parallel_for (size_t(0),nnX.size()*nnO.size(),[&](size_t iXO){//the parallelized tasks should be smaller... parallel_for over the steps?
			int iX = iXO % nnX.size();
			int iO = iXO / nnX.size();
			Battle b(nnX[iX].first, nnO[iO].first);
			if (iXO == 0){//Output the first match of every generation (nnO[0] vs nnX[0])
				cout << (char)0xda; for (int i = 0; i < Battle::fieldsize; i++){ cout << (char)0xc4; } cout << (char)0xbf << endl;
				for (int i = 0; i < steps_per_battle && b.ORobots.size() > 0; i++){ b.step(); if (i % 2 == 0){ b.output(); /*cout << b.getCurrentOScore(); cin.get();*/ } } b.output();
				cout << (char)0xc0; for (int i = 0; i < Battle::fieldsize; i++){ cout << (char)0xc4; } cout << (char)0xd9 << endl;
				cout << endl
					<< "Done displaying " << generation << ":" << endl
					<< "  Steps per battle: " << steps_per_battle << endl
					<< "  OLifetimeSum: " << b.getCurrentOScore() << "    " << endl
					<< "  XTraveled: " << b.getXTraveled() << "    " << endl
					<< "  OTraveled: " << b.getOTraveled() << "    " << endl;
			}
			else{
				for (int i = 0; i < steps_per_battle && b.ORobots.size() > 0; i++)b.step();
			}
			nnX[iX].second.first += b.getCurrentOScore();
			nnO[iO].second.first += b.getCurrentOScore();
			nnX[iX].second.second += b.getXTraveled();
			nnO[iO].second.second += b.getOTraveled();
		});

		fout << generation << " X: " << nnX[0].first.exportString() << endl
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

	fout << endTimestamp(startTime);
	cin.get();
	return 0;
}

