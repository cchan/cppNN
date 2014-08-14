#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdio.h>

using namespace std;

double rand(double a,double b){
	return (b-a)*((double)rand()/(double)RAND_MAX)+a;
}


//I need a feedback system JUST to maintain homeostasis. It's an unstable equilibrium.
//Like, if there's too few firing encourage more. Can I PID this? Can I PID the whole feedback system?
//Can the feedback be just INTO a neuron somewhere in the network? But the problem is that how would the system know to interpret that as pain?\
//This should be simpler!! :(


class Neuron{
public:
	bool fired, newfired;
	bool constant;
	int n;
	double voltage, threshold;
	vector<double> weights;//from
	Neuron(){}
	Neuron(bool f){ constant = true; fired = f; voltage = threshold = 3.14159; }
	Neuron(int num){
		n=num;
		constant=false;
		fired=newfired=false;
		voltage=rand(-1,1);
		threshold=rand(-1,1);
		weights.resize(n);
		for(int i=0;i<n;i++)weights[i]=rand(-1,1);
	}
	//Neuron(int n, Neuron r){ fired = r.fired; newfired = r.newfired; constant = r.constant; n = r.n; voltage = r.voltage; threshold = r.threshold; }
	void step(vector<Neuron> net){
		if(constant)return;
		
		newfired = false;
		for(int i=0;i<n;i++){
			if(net[i].fired){
				voltage+=weights[i];
				weights[i]*=1.01;
			}
			else {
				weights[i]*=0.99;
			}
		}
		
		if(voltage>threshold){
			//cout<<"(wtf "<<voltage<<","<<threshold<<")";
			newfired=true;
			voltage=0;
			threshold*=0.99;
		}
		voltage/=2.0;
	}
	void update(){
		fired = newfired;
	}
};

int main(){
	srand(time(NULL));

	vector<Neuron> net;

	net.push_back(Neuron(true));

	int n = 5;

	for (bool b : {true, false, true, true})net.push_back(Neuron(b));
	for(int i=0;i<n;i++)net.push_back(Neuron(n));

	for (int gen = 0; gen<50; gen++){
		for (int i = 0; i<net.size(); i++)net[i].step(net);
		for (int i = 0; i<net.size(); i++)net[i].update();
		for(int i=0;i<net.size();i++)printf("(%f>%f)? ",net[i].voltage,net[i].threshold);
		cout<<endl;
	}
	cin.get();
}

