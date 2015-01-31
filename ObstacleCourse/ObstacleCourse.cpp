#include <iostream>
#include <cmath>
#include "../common/rand11.h"

using namespace std;

const double PI = 3.14159265358979323846264338;

class Point{
public:
	double x, y;
	Point(){}
	Point(double newx, double newy){ x = newx; y = newy; }
	Point operator+(Point p){ return Point(x + p.x, y + p.y); }
	Point operator-(Point p){ return Point(x - p.x, y - p.y); }
	Point rotate(double radians){
		return Point(x * cos(radians) - y * sin(radians), y * cos(radians) + x * sin(radians));
	}
	double magn(){ return sqrt(x*x + y*y); }
	double dist(Point p){ return (*this - p).magn(); }
};
class Ray{
public:
	Point a, b; //Ray with tail at a, passing thru b.
	Ray(Point newa, Point newb){
		a = newa; b = newb;
	}
	Ray rotate(double radians){ //Counterclockwise, as usual.
		return Ray(a, a + (b - a).rotate(radians));
	}
	double checkSide(Point p){
		//Finds the sign of the point relative to this line (as in, finds what side of the line it's on) - dotproduct with a perpendicular
		//The left side is negative, and the right side is positive, and on it is zero.
		//https://stackoverflow.com/questions/10906381/how-to-find-out-if-a-ray-intersects-a-rectangle
		return (p.x - a.x)*(b.y - a.y) + (p.y - a.y)*(a.x - b.x);
	}
};
class SonarServo{//Trying to behave as closely as possible to a Lego Sonar Sensor, so this can be applied to FTC possibly.
private:
	Ray baseHeading;
	double servoAngle = 0;
	const double spread = PI / 8;//45 degrees total spread
	Ray getRotatedDir(){
		return baseHeading.rotate(servoAngle);
	}
	bool pointInCone(Point p){
		return baseHeading.rotate(servoAngle + spread).checkSide(p) > 0 //right side of CCW-rotated
			&& baseHeading.rotate(servoAngle - spread).checkSide(p) < 0;//left side of CW-rotated
	}
public:
	//moveServoToValue
	unsigned char distanceReading(Point p){ //0 to 255
		double dist = baseHeading.a.dist(p);

		if (!pointInCone(p))return 255; //If it's out of cone, return error
		if (dist > 100) return 255; //If it's out of range, return error.
		if (dist < 5) return 255; //If it's way too close, return error.
		if (rand01() > 0.95) return 255; //and randomly have errors
		if (dist > 80 && rand01() > 0.8) return 255; //plus have more errors at larger distances

		if (dist < 25) dist = 25 + rand11() * 2; //If too close, don't return the right values.

		return round(dist + rand11()/2); //be reasonably accurate, but never exact
	}
	void moveTo(Ray posAndRot){

	}
	unsigned char servoRelativeHeading(){ //0 to 255

	}
};


class Block{
	double centerX, centerY, theta, diameter;
	double intersect(SonarServo ss){ //returns point of intersection between line and block nearest to ax, ay of line. returns std::nan (use std::isnan()) when doesn't hit.


		//check if NaN is supported? because else returns 0
	

};

class Field{

};


int main(){
	
}

