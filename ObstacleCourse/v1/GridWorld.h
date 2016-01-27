#include <cmath>
#include <vector>
#include <cassert>
#include <string>
#include "../../common.h"
#include "../../LayeredNeural_Backprop_Floats/NeuralNetwork.h"

class Robot{
private:
	NeuralNetwork& brain;

	//double collabValue; //Outputs a value which is inputted into all other NNs of its type, so it can sort of "collaborate". Sequential vs. layered updating will be awkward.

public:
	int x, y;
	int FOVMax, FOVMin;
	Robot(NeuralNetwork& _brain, int _x, int _y, int _team, int _FOVMin, int _FOVMax);
	Robot(const Robot& other);
	Robot operator=(const Robot& other);
	int step(const std::vector<std::vector<int> >& field, int stepnum);

};

class XRobot : public Robot{
public:
	XRobot(NeuralNetwork& _brain, int _x, int _y, int _team, int _FOVMin, int _FOVMax) : Robot(_brain, _x, _y, _team, _FOVMin, _FOVMax){}
};

class ORobot : public Robot{
public:
	ORobot(NeuralNetwork& _brain, int _x, int _y, int _team, int _FOVMin, int _FOVMax) : Robot(_brain, _x, _y, _team, _FOVMin, _FOVMax){}
	bool isEaten(const std::vector<XRobot>& XRobots) const{ //Within 1, in both X and Y coords.
		for (XRobot r : XRobots)
			if (abs(x - r.x) <= 1 && abs(y - r.y) <= 1)
				return true;
		return false;
	}
};



class Battle{

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

public:
	const static int fieldsize = 20;
	const int startingX = 2, startingO = 4;
	const int FOVMax = 4, FOVMin = -4;
	int OScore = 0, OTraveledScore = 0, XTraveledScore = 0, stepnum = 0;
	std::vector<int> OLifetimes;
	std::vector<std::vector<int> > field;

	std::vector<XRobot> XRobots;
	std::vector<ORobot> ORobots;

	Battle(NeuralNetwork& nnX, NeuralNetwork& nnO);
	std::vector<std::vector<int> > getField();
	int getCurrentOScore();
	int getOTraveled(){return OTraveledScore;}
	int getXTraveled(){return XTraveledScore;}
	void step();
	void output(std::ostream& cout);//Outputs the Xs, Os, and uses '.' for anything in the field of view.
};


void replay(std::ostream& cout, std::string nnXexport, std::string nnOexport, int steps_per_battle, int delay);




