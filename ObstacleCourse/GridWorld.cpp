#include "GridWorld.h"


Robot::Robot(NeuralNetwork& _brain, int _x, int _y, int _team, int _FOVMin, int _FOVMax) : brain(_brain){
	x = _x;
	y = _y;
	FOVMin = _FOVMin;
	FOVMax = _FOVMax;
}
Robot::Robot(const Robot& other) : brain(other.brain){
	x = other.x;
	y = other.y;
	FOVMin = other.FOVMin;
	FOVMax = other.FOVMax;
}
Robot Robot::operator=(const Robot& other){
	brain = other.brain;
	x = other.x;
	y = other.y;
	FOVMin = other.FOVMin;
	FOVMax = other.FOVMax;
	return other;
}

int Robot::step(const std::vector<std::vector<int> >& field, int stepnum){ //Field: +1 if X team, -1 if O team, 0 if nothing. Same width and height.
	assert(brain.getOutputSize() == 2); //Because step() wouldn't work otherwise.
	assert(brain.getInputSize() == (FOVMax - FOVMin + 1)*(FOVMax - FOVMin + 1) + 3); //Make sure it's the right inputsize too. 

	std::vector<double> proximity;
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

	std::vector<double> newpos = brain.thresholding(brain.frontprop(proximity));

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



Battle::Battle(NeuralNetwork& nnX, NeuralNetwork& nnO){
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

	OLifetimes = std::vector<int>(ORobots.size(), 0);

	field = getField();
}
std::vector<std::vector<int> > Battle::getField(){
	std::vector<std::vector<int> > field(fieldsize);
	for (auto &v : field)v = std::vector<int>(fieldsize);//Init to zero
	for (const Robot &r : XRobots)field[r.x][r.y] = 1;//Add XRobots
	for (const Robot &r : ORobots)field[r.x][r.y] = -1;//Add ORobots
	return field;
}
int Battle::getCurrentOScore(){
	int currOScore = OScore;
	for (int i = 0; i < ORobots.size(); i++)
		currOScore += OLifetimes[i];
	return currOScore;
}
void Battle::step(){
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
void Battle::output(std::ostream& cout){//Outputs the Xs, Os, and uses '.' for anything in the field of view.
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
		cout << (char)0xb3 << std::endl;
	}
}


void replay(std::ostream& cout, std::string nnXexport, std::string nnOexport, int steps_per_battle, int delay){
	NeuralNetwork nnX(nnXexport), nnO(nnOexport);
	Battle b(nnX, nnO);

	cout << (char)0xda; for (int i = 0; i < Battle::fieldsize; i++){ cout << (char)0xc4; } cout << (char)0xbf << std::endl;
	COORD coord = getConsoleCoord();
	for (int i = 0; i < Battle::fieldsize; i++)cout << std::endl;
	cout << (char)0xc0; for (int i = 0; i < Battle::fieldsize; i++){ cout << (char)0xc4; } cout << (char)0xd9 << std::endl;

	for (int i = 0; i < steps_per_battle && b.ORobots.size() > 0; i++){
		//cin.get();
		setConsoleCoord(coord);
		b.step();
		b.output(cout);
		for (int j = 0; j < delay*1000000; j++);
	}
}



