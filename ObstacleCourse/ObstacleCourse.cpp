#include <iostream>
#include <cmath>

using namespace std;

const double PI = 3.14159265358979323846264338;

class SonarRay{ //It's a ray, so it only goes in one direction. A bit of bounds-checking, that's all.
public:
	double ax, ay, bx, by;
	double dotProductAgainst(double px, double py){
		//Finds the sign of the point relative to this line (as in, finds what side of the line it's on)
		//https://stackoverflow.com/questions/10906381/how-to-find-out-if-a-ray-intersects-a-rectangle
		return (px - ax)*(by - ay) + (py - ay)*(ax - bx);
	}
	double isInFront(double px, double py){
		//Checks that it's actually in front of the ray, not behind it.

	}
};

class SonarBeam{
	const double spreadFromCenter = PI/8; //that's 45 degrees total.

};

class Block{
	double centerX, centerY, theta, diameter;
	double intersect(SonarLine sr){ //returns point of intersection between line and block nearest to ax, ay of line. returns std::nan (use std::isnan()) when doesn't hit.


		//check if NaN is supported? because else returns 0
		return std::nan(NULL);
	}
};

class Field{

};


int main(){
	
}

