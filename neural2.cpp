#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>

using namespace std;

float getRand(float max){
	return max*((float)rand())/RAND_MAX;
}


class Neuron{
public:
	float val;
	float threshold;
	bool constant;
	vector<float>weights;
	
	/*default*/Neuron(){}
	/*copy*/Neuron(const Neuron& n){val=n.val;threshold=n.threshold;constant=n.constant;neuronsTo=n.neuronsTo;weights=n.weights;}
	Neuron(float v){//when you want it to be an input neuron
		constant=true;
		val=v;
	}
	void connect(int netSize){//Adding it to the network.
		constant=false;
		
		val=getRand(netSize);
		threshold=netSize/2;
		
		weights.resize(netSize);
		for(int i=0;i<weights.size();i++)weights[i]=getRand(1);
	}
	void fire(vector<Neuron> net, float score){
		if(constant){//This is bad. That connection can't evolve.
			//for(int i=0;i<net.size();i++)net[i].val+=val;
		}
		else{//Needs to use the neurons FROM; doesn't matter what TO.
			if(val>threshold){
				for(int i=0;i<neuronsTo.size();i++)neuronsTo[i].val+=weights[i];
				val=0;
			}
			else val/=1.5;
			
			for(int i=0;i<weights.size();i++)//update weights based on score
				weights[i]+=getRand(0.1)+score*0.1-0.1;
			
			//change threshold based on how much i've been firing lately (always must be >0)
			//if val==0 means just fired.
			threshold+=(val<0.01)?neuronsTo.size()/5.0:-neuronsTo.size()/20.0;
			threshold=threshold>0?threshold:0;
		}
		
		
		//update weights somehow, in very small increments
	}
};

int main(){
	srand(time(NULL));
	
	int n=100;
	vector<Neuron> neurons;
	
	neurons.push_back(Neuron(getRand(1)));
	neurons.push_back(Neuron(getRand(1)));
	neurons.resize(n);
	for(int i=2;i<n;i++)neurons[i]=neuron;//huh... won't see the ones constructed previously.
	
	float score=neurons[2].val-neurons[0].val-neurons[1].val;
	for(int i=0;i<1000;i++){
		for(int i=0;i<neurons.size();i++)neurons[i].fire(score);
	}
	
	cout<<neurons[0].val<<" + "<<neurons[1].val<<" = "<<neurons[2].val;
	
	//How many "updates" of the brain will be allowed? Huh... the brain is even more complex than this model - it works in the temporal dimension too.
	//Maybe when it indicates with one more neuron that the answer is "ready".
}


