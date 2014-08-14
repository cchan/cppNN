#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdio.h>

using namespace std;

double rand(double a, double b){
	return (b - a)*((double)rand() / (double)RAND_MAX) + a;
}


//I need a feedback system JUST to maintain homeostasis. It's an unstable equilibrium.
//Like, if there's too few firing encourage more.


class Neuron{
public:
	int PotassiumIonsOut,
		PotassiumIonsIn,
		SodiumIonsOut,
		SodiumIonsIn;
};

int main(){
	srand(time(NULL));

	//You get the terrible idea.
}
