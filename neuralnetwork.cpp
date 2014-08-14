#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>

using namespace std;


float getRand(){//Float from 0 to 1.
	return ((float)rand())/RAND_MAX;
}
float norm(float a){
	if(a>1)return 1.0;
	if(a<0)return 0.0;
	return a;
}

class Connection;

class Neuron{
public:
	float val;
	float nextVal;
	bool isConstant;
	vector<Connection> cons;
	
	Neuron(){}
	
	Neuron(vector<Neuron> n){
		isConstant=false;
		val=getRand();
		nextVal=0;
		cons.resize(n.size());
		for(int i=0;i<n.size();i++)
			cons=new Connection(n[i]);
	}
	void next(float score){
		for(int i=0;i<cons.size();i++)
			cons[i].next(score);
		nextVal=0;
		float totalWeight=0;
		for(int i=0;i<cons.size();i++){
			nextVal+=cons.from.val*cons.weight;
			totalWeight+=cons.weight;
		}
		nextVal/=totalWeight;
	}
	void nextFinish(){
		val=nextVal;
		nextVal=0;
	}
};

class Connection{
public:
	Neuron from;
	float weight;
	//how good it "has been"?
	Connection(){}
	Connection(Neuron f){
		from=f;
		weight=getRand();
	}
	void next(float score){//score also from 0 to 1.
		weight=norm(weight+(getRand()-0.5)*(1-score));
		//Changes more if it's a bad score. - but then it does that on EVERY thing.
	}
};

int main(){
	srand(time(NULL));
	
	vector<Neuron> neurons;
	float a=getRand(),b=getRand();
	
	int n=10;
	neurons.resize(n);
	for(int i=0;i<n;i++)
		neurons[i]=new Neuron(neurons);
	
	while(true){
		neurons[0].isConstant=neurons[1].isConstant=true;
		neurons[0].val=a;neurons[1].val=b;
		
		float score=neurons[3].val-a-b;
		score*=score;
		
		for(int i=0;i<neurons.size();i++)neurons[i].next(score);
		for(int i=0;i<neurons.size();i++)neurons[i].nextFinish();
	}
	
	
	
	//How many "updates" of the brain will be allowed? Huh... the brain is even more complex than this model - it works in the temporal dimension too.
	//Maybe when it indicates with one more neuron that the answer is "ready".
}


