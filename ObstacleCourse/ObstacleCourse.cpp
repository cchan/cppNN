#include <iostream>
#include <cmath>
#include <vector>
#include "../common.h"
#include "time.h"

using namespace std;

const double PI = 3.14159265358979323846264338;

class Point{
public:
	double x, y;
	Point(){}
	Point(double newx, double newy){ x = newx; y = newy; }
	Point operator+(const Point& p) const{ return Point(x + p.x, y + p.y); }
	Point operator-(const Point& p) const{ return Point(x - p.x, y - p.y); }
	Point rotate(double radians) const{
		return Point(x * cos(radians) - y * sin(radians), y * cos(radians) + x * sin(radians));
	}
	double magn() const{ return sqrt(x*x + y*y); }
	double dist(const Point& p) const{ return (*this - p).magn(); }
	double theta() const{ return atan2(y, x); }
};

class Ray{
	Point a, b; //Ray with tail at a, passing thru b.
public:
	Ray(){}
	Ray(Point newa, Point newb){
		a = newa; b = newb;
	}
	Ray(Point origin, double th){
		a = origin;
		b = origin + Point(1, 0).rotate(th);
	}
	double theta() const{
		return (b - a).theta();
	}
	Point origin() const{ return a; }
	double x() const{ return a.x; }
	double y() const{ return a.y; }
	/*Ray translateXYT(double x, double y, double t){
		
	}*/
	Ray rotate(double radians) const{ //Counterclockwise, as usual.
		return Ray(a, a + (b - a).rotate(radians));
	}
	double checkSide(Point p) const{
		//Finds the sign of the point relative to this line (as in, finds what side of the line it's on) - dotproduct with a perpendicular
		//The left side is negative, and the right side is positive, and on it is zero.
		//https://stackoverflow.com/questions/10906381/how-to-find-out-if-a-ray-intersects-a-rectangle
		return (p.x - a.x)*(b.y - a.y) + (p.y - a.y)*(a.x - b.x);
	}
};

class SonarServo{//Trying to behave as closely as possible to a Lego Sonar Sensor, so this can be applied to FTC possibly.
private:
	const double spread = PI / 8;//45 degrees total spread
	const double minServoAngle = 0;
	const double maxServoAngle = PI;
	const int minServoValue = 0;
	const int maxServoValue = 255;

	Ray posdir;
	double currentServoAngle;
	Ray getRotatedDir() const{
		return posdir.rotate(currentServoAngle);
	}
	bool pointInCone(Point p) const{
		return posdir.rotate(currentServoAngle + spread).checkSide(p) > 0 //right side of CCW-rotated
			&& posdir.rotate(currentServoAngle - spread).checkSide(p) < 0;//left side of CW-rotated
	}
public:
	SonarServo(Ray newBaseHeading){
		currentServoAngle = 0;
		posdir = newBaseHeading;
	}
	//moveServoToValue
	double actualDistanceReading(Point p) const{
		return posdir.origin().dist(p);
	}
	double distanceReading(Point p) const{ //0 to 255
		double dist = actualDistanceReading(p);

		if (!pointInCone(p))return 255; //If it's out of cone, return error
		if (dist > 100) return 255; //If it's out of range, return error.
		if (dist < 5) return 255; //If it's way too close, return error.
		if (rand01() > 0.95) return 255; //and randomly have errors
		if (dist > 80 && rand01() > 0.8) return 255; //plus have more errors at larger distances

		if (dist < 25) dist = 23 + rand11() * 2; //If too close, level out at around 25

		return round(dist + rand11() / 2); //be reasonably accurate, but never exact
	}
	void moveTo(Ray newposdir){
		posdir = newposdir;
	}
	double servoRelativeEncoderHeading() const{ //0 to 255
		return minServoValue + (maxServoValue - minServoValue) * (currentServoAngle - minServoAngle) / (maxServoAngle - minServoAngle);
	}
};

class FieldObject{
protected:
	Ray posdir; //x, y position, and angle
	Ray veldir; //x, y, theta components to velocity, per second.
	double radius;
public:
	virtual void step() = 0;
	void pos(const Ray& newposdir){
		posdir = newposdir;
	}
	void vel(const Ray& newveldir){
		veldir = newveldir;
	}
	double SonarServoDistance(const SonarServo& ss) const{
		//Nothing is a rectangle, just a point (well, a circle, since passing within *radius*1 + *radius*2 will be a collision
		return ss.distanceReading(posdir.origin());
	}
	bool colliding(const FieldObject& other) const{
		return (posdir.origin() - other.posdir.origin()).magn() < radius + other.radius;
	}
};

class Robot : public FieldObject{
private:
	vector<SonarServo> sonars;
public:
	Robot(){}
	Robot(Ray newposdir, Ray newveldir){
		for (int i = 0; i < 4; i++)//Four sonars with base headings orthogonal.
			sonars.push_back(posdir.rotate(i * PI / 2.0));
		posdir = newposdir;
		veldir = newveldir;
	}
	void step(){
		
	}
};

class Obstacle : public FieldObject{
	Obstacle(){};
	void step(){}
};


class Field{
	Robot robot;
	vector<Obstacle> obstacles;
	Point startPoint;
	Point targetPoint;
	Field(){ Robot robot(Ray(Point(1, 1), PI / 4), Ray(Point(0, 0), 0)); }

};


int main(){
	;
}

