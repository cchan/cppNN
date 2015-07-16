#include <cmath>
#include <vector>
#include <cassert>
#include <string>
#include "../../common.h"
#include "../../LayeredNeural_Backprop_Floats/NeuralNetwork.h"

class PosVel;
class Robot;
class XRobot;
class ORobot;
class Battle;



class PosVel{
private:
	int px, py,
		vx, vy,
		maxX, maxY;

	void normalizeXY(){
		if (px < 0) px = 0;
		else if (px > maxX) px = maxX;
		if (py < 0) py = 0;
		else if (py > maxY) py = maxY;
	}

public:
	PosVel(){}
	PosVel(int _px, int _py, int _vx, int _vy, int _maxX, int _maxY){
		px = _px, py = _py;
		vx = _vx, vy = _vy;
		maxX = _maxX, maxY = _maxY;
		assert(maxX >= 0 && maxY >= 0);
		normalizeXY();
	}

	int getPX() const{ return px; }
	int getPY() const{ return py; }
	//int getVX() const{ return vx; }
	//int getVY() const{ return vy; }

	double speed() const{ return sqrt(vx*vx + vy*vy); }

	void moveForward(){ px += vx; py += vy; normalizeXY(); }
	void moveBackward(){ px -= vx; py -= vy; normalizeXY(); }
	void translateLeft(){ px -= vy; py += vx; normalizeXY(); }
	void translateRight(){ px += vy; py -= vx; normalizeXY(); }
	void rotate90Left(){ int tmp = vx; vx = -vy; vy = tmp; }
	void rotate90Right(){ int tmp = vx; vx = vy; vy = -tmp; }
};


class Robot{
private:
	NeuralNetwork& brain;
	//double collabValue; //Outputs a value which is inputted into all other NNs of its type, so it can sort of "collaborate". Sequential vs. layered updating will be awkward.

public:
	int id;
	PosVel currPV, nextPV;
	Robot(NeuralNetwork& _brain, PosVel start, int _id); //intentionally copying PosVel so can init multiple with one
	//Robot(const Robot& other);
	//Robot operator=(const Robot& other);
	double step(const std::vector<XRobot>& XRobots, const std::vector<ORobot>& ORobots);
};

class XRobot : public Robot{
public:
	XRobot(NeuralNetwork& _brain, PosVel start, int _id) : Robot(_brain, start, _id){}
};

class ORobot : public Robot{
public:
	ORobot(NeuralNetwork& _brain, PosVel start, int _id) : Robot(_brain, start, _id){}
	bool isEaten(const std::vector<XRobot>& XRobots) const{ //Within 1, in both X and Y coords. So within that 3x3 square.
		for (XRobot r : XRobots)
			if (abs(currPV.getPX() - r.currPV.getPX()) <= 1 && abs(currPV.getPY() - r.currPV.getPY()) <= 1)
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
	int XHowMany, OHowMany;

	int OScore = 0, stepnum = 0;
	double OTraveledScore = 0, XTraveledScore = 0;
	std::vector<int> OLifetimes;

	std::vector<XRobot> XRobots;
	std::vector<ORobot> ORobots;

	Battle(int _XHowMany, int _OHowMany, NeuralNetwork& nnX, NeuralNetwork& nnO);
	std::vector<std::vector<int> > getField();
	int getCurrentOScore();
	double getOTraveled(){return OTraveledScore;}
	double getXTraveled(){return XTraveledScore;}
	void step();
	void output(std::ostream& cout);//Outputs the Xs, Os, and uses '.' for anything in the field of view.
};


//void replay(std::ostream& cout, std::string nnXexport, std::string nnOexport, int steps_per_battle, int delay);




