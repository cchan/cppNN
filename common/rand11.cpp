#include "rand11.h"

double rand11(){
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<double> d(-1.0, 1.0);
	return d(gen);
}
double rand01(){
	return (rand11() + 1) / 2;
}
int randInt(int a, int b){
	return floor(a + (b - a + 1) * rand01());
}

