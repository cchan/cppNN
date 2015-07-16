#include "GridWorld.h"


Robot::Robot(NeuralNetwork& _brain, int _x, int _y, int _id, int _fieldsize) : brain(_brain){
	x = _x;
	y = _y;
	id = _id;
	dir = XYPair(1, 0);
	fieldsize = _fieldsize;
}
/*Robot::Robot(const Robot& other) : brain(other.brain){
	x = other.x;
	y = other.y;
	id = other.id;
	vx = 1;
	vy = 0;
	fieldsize = other.fieldsize;
}
Robot Robot::operator=(const Robot& other){
	brain = other.brain;
	x = other.x;
	y = other.y;
	id = other.id;
	vx = other.vx;
	vy = other.vy;
	fieldsize = other.fieldsize;
	return other;
}*/
double Robot::step(const std::vector<XRobot>& XRobots, const std::vector<ORobot>& ORobots){ //Field: +1 if X team, -1 if O team, 0 if nothing. Same width and height.
	assert(brain.getOutputSize() == 2); //Because step() wouldn't work otherwise.
	assert(brain.getInputSize() == (XRobots.size() + ORobots.size()) * 2 + 2); //Make sure it's the right inputsize too. 

	std::vector<double> indata;
	for (Robot r : XRobots){
		indata.push_back(r.x);
		indata.push_back(r.y);
	}
	for (Robot r : ORobots){
		indata.push_back(r.x);
		indata.push_back(r.y);
	}
	indata.push_back(this.x); indata.push_back(this.y);
	indata.push_back(dir.getX()); indata.push_back(dir.getY());
	//indata.push_back(timestep); // Let's see if stateless can work.


	//Put it in to the network
	std::vector<double> newpos = brain.frontprop(indata);
	
	
	double distTraveled = 0;

	//Tank drive!
	if (newpos[0] > 0 && newpos[1] > 0){ x += dir.getX(); y += dir.getY(); distTraveled = dir.magn(); } //forward move
	else if (newpos[0] < 0 && newpos[1] > 0){ dir.rotate90Left(); } //left rotate 90
	else if (newpos[0] > 0 && newpos[1] < 0){ dir.rotate90Right(); } //right rotate 90
	else { x -= dir.getX(); y -= dir.getY(); distTraveled = dir.magn(); } //backward move
	//Unfortunately they do collide. But that's fine.

	normalizeXY();
	
	return distTraveled;
	//(the return values are for distance-traveled tracking)
}

void Robot::normalizeXY(){
	if (x > fieldsize - 1) x = fieldsize - 1;
	if (x < 0) x = 0;
	if (y > fieldsize - 1) y = fieldsize - 1;
	if (y < 0) y = 0;
}



Battle::Battle(int _XHowMany, int _OHowMany, NeuralNetwork& nnX, NeuralNetwork& nnO){
	XHowMany = _XHowMany; OHowMany = _OHowMany;

	int x, y;
	for (int i = 0; i < XHowMany; i++){
		x = fieldsize / 4;
		y = fieldsize / (XHowMany + 1) * (1 + i);

		XRobots.push_back(XRobot(nnX, x, y, i, fieldsize));
	}
	for (int i = 0; i < OHowMany; i++){
		x = 3 * fieldsize / 4;
		y = fieldsize / (OHowMany + 1) * (1 + i);

		ORobots.push_back(ORobot(nnO, x, y, i, fieldsize));
	}

	OLifetimes = std::vector<int>(ORobots.size(), 0);
}
int Battle::getCurrentOScore(){
	int currOScore = OScore;
	for (size_t i = 0; i < ORobots.size(); i++)
		currOScore += OLifetimes[i];
	return currOScore;
}
void Battle::step(){
	//Eating
	for (int i = ORobots.size() - 1; i >= 0; i--){//Backwards, so that the downshifting when deleted doesn't affect anything.
		if (ORobots[i].isEaten(XRobots)){
			OScore += OLifetimes[i] * OLifetimes[i]; // The longer the better
			OLifetimes[i] = 0;
			ORobots[i].x = randInt(0, fieldsize - 1);
			ORobots[i].y = randInt(0, fieldsize - 1);
		}
	}

	//Moving XRobots
	for (XRobot &r : XRobots){
		int oldx = r.x, oldy = r.y;
		XTraveledScore += r.step(XRobots, ORobots);
	}
	
	//Moving ORobots
	for (size_t i = 0; i < ORobots.size(); i++){
		OTraveledScore += ORobots[i].step(XRobots, ORobots);
		OLifetimes[i]++;
	}
}
void Battle::output(std::ostream& cout){//Outputs the Xs and Os on the field.
	std::vector<std::vector<int> > field;
	field.resize(fieldsize);
	for (std::vector<int>& v : field) v.resize(fieldsize);

	for (const XRobot& r : XRobots)if (field[r.x][r.y] != 0)field[r.x][r.y] = 2; else field[r.x][r.y] = 1;
	for (const ORobot& r : ORobots)if (field[r.x][r.y] != 0)field[r.x][r.y] = 2; else field[r.x][r.y] = -1;
	for (int i = 0; i < fieldsize; i++){
		cout << (char)0xb3; // OEM extended ascii
		for (int j = 0; j < fieldsize; j++){
			if (field[i][j] == 0)cout << ' ';
			else if (field[i][j] == -1)cout << 'O';
			else if (field[i][j] == 1)cout << 'X';
			else if (field[i][j] == 2)cout << '@';//Whenever there's overlap
			else { cout << "Invalid value: " << field[i][j]; assert(false); }
		}
		cout << (char)0xb3 << std::endl;
	}
}


/*void replay(std::ostream& cout, std::string nnXexport, std::string nnOexport, int steps_per_battle, int delay){
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
*/


