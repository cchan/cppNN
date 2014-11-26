#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>
#include <string>

using namespace std;

double operator*(const vector<double>& a, const vector<double>& b){
	double s = 0;
	int size = min(a.size(), b.size());
	for (int i = 0; i < size; i++)
		s += a[i] * b[i];
	return s;
}
vector<double> operator+(const vector<double>& a, const vector<double>& b){
	vector<double> c(0, min(a.size(), b.size()));
	for (int i = 0; i < c.size(); i++)
		c[i] = a[i] + b[i];
	return c;
}
vector<double> operator*(double k, const vector<double>& a){
	vector<double> c(a.size());
	for (int i = 0; i < c.size(); i++)
		c[i] = a[i] * k;
	return c;
}
vector<double> operator*(const vector<double>& a, double k){ return k*a; }
vector<double> operator-(const vector<double>& a, const vector<double>& b){
	return a + b * -1;
}
vector<double> operator/(double k, const vector<double>& a){ return a * (1.0/k); }
vector<double> operator/(const vector<double>& a, double k){ return a * (1.0/k); }
vector<double> operator-=(vector<double>& a, const vector<double>& b){
	for (int i = 0; i < min(a.size(), b.size()); i++)
		a[i] -= b[i];
	return a;
}
ostream& operator<<(ostream& out, const vector<double>& a){
	int origprec = cout.precision();
	cout.precision(5);	//Variation after each backprop is less than 0.1% (at least in one of the properly configured ones), so this pretty much has three sigfigs.
	out << "[" << a[0];
	for (int i = 1; i < a.size(); i++)
		out << "," << a[i];
	out << "]";
	cout.precision(origprec);
	return out;
}

vector<double> sampleTanks(int actualpop, int sample){
	assert(actualpop >= sample);

	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<int> tanks(1, actualpop);

	vector<int> tanknums(sample);
	for (int i = 0; i < sample; i++){
		int rand;
		do{
			rand = tanks(gen);
			for (int j = 0; j < sample; j++) if (rand == tanknums[j]) continue;
			break;
		} while (true);
		tanknums[i] = rand;
	}
	sort(tanknums.begin(), tanknums.end());

	vector<double> tanknums_double(tanknums.size());
	for (int i = 0; i < tanknums.size(); i++)
		tanknums_double[i] = tanknums[i];

	return tanknums_double;
}

//Interesting results:
//For sum of scores, sum of squared scores, sum of cubed scores:
//Given a single sample, the best thing you can possibly do in all cases is to multiply by 3/2, subtract 1.
//Given two, multiply by 4/3, subtract 1.
//Given three, ... etc


int main(){
	static double weights98bias = 0;//~19.5 is the best for 7/1000. 190 is the best for 7/10000.
	const int sample = 7;//Smaller => backprop better; larger => scaling better (very influential)
	const int actualpop = 1000; //Smaller => scaling better; larger => backprop better (not that influential)
	const int backprop_rounds = 100000;
	const int score_rounds = 1000000;
	cout.precision(5);
	//cout.setf(ios::fixed, ios::floatfield);

	static random_device rd;
	static mt19937 gen(rd());
	static uniform_real_distribution<double> preweight(-0.001, 0.001);

	vector<vector<double> > weightsets;
	vector<string> names;
	vector<double> weightable(sample + 1), weightsavg2(sample + 1), weightsmaxmin(sample + 1), weights87(sample + 1), weights871(sample + 1), weights98(sample + 1), weights981(sample + 1);
	for (int i = 0; i < sample + 1; i++) weightable[i] = preweight(gen);
	for (int i = 0; i < sample + 1; i++) weightsavg2[i] = 2.0 / sample;
	weightsmaxmin[0] = weightsmaxmin[sample - 1] = 1;
	weights87[sample - 1] = (sample + 1.0) / sample;//Multiply maximum by a factor to extend it a little bit.
	weights871 = weights87;	weights871[sample] = -1;//Bias is -1
	weights98[sample - 1] = (sample + 2.0) / (sample + 1.0); weights98[sample] = -weights98bias;
	weights981 = weights98;	weights981[sample] = -(weights98bias-1);
	//-19.5 is the best bias for 7/1000

	names.push_back("Backpropagated"); weightsets.push_back(weightable);
	names.push_back("Avg*2"); weightsets.push_back(weightsavg2);
	names.push_back("Max+min"); weightsets.push_back(weightsmaxmin);
	names.push_back("Scaled Max"); weightsets.push_back(weights87);
	names.push_back("Scaled Max -1"); weightsets.push_back(weights87);
	names.push_back("Scaled++ Max, bias -" + to_string(weights98bias)); weightsets.push_back(weights98);
	names.push_back("Scaled++ Max, bias -" + to_string(weights98bias-1)); weightsets.push_back(weights981);

	vector<double> scores(weightsets.size());

	for (unsigned long long n = 0; true; n++){
		//weights98bias++;
		weightsets[6][sample] = (weightsets[5][sample] = -weights98bias) + 1;
		names[5]=("Scaled++ Max, bias -" + to_string(weightsets[5][sample]));
		names[6]=("Scaled++ Max, bias -" + to_string(weightsets[6][sample]));
		vector<double> tanks = sampleTanks(actualpop, sample);
		tanks.push_back(1);//Bias term
		
		//assert(tanks.size() == sample + 1);
		//assert(weightsets[0].size() == sample + 1);
		if (n % ((backprop_rounds+score_rounds) / 80) == ((backprop_rounds+score_rounds / 80) - 1 || n == 0)) cout << '.';
		if (n < backprop_rounds){ //Backpropagate for ? rounds
			float learningrate = 1.0 / (40 * actualpop * (n + actualpop));//I evidently do not understand Learning Rate properly; it's really weird what happens when I adjust.
			weightsets[0] -= tanks * (weightsets[0] * tanks - actualpop) * learningrate;
			//               ^^^^^ Is it actually 'tanks' or is it delta?
		}
		else if (n < backprop_rounds + score_rounds) {//Score for ? rounds
			for (int i = 0; i < scores.size(); i++){
				double score = (weightsets[i] * tanks - actualpop);
				scores[i] += abs(score); // Interestingly, with squared scores, in configuration 7/1000, the neuralnet's advantage over max871 is insignificant.
			}
		}
		else
			break;
	}

	int minind = 0;
	double minscore = scores[0];
	for (int i = 1; i < weightsets.size(); i++)
		if (minscore > scores[i]){ minind = i; minscore = scores[i]; }
	for (int i = 0; i < weightsets.size(); i++)
		if (i == minind)
			cout << names[i] << ": minimum" << endl;
		else
			cout << names[i] << ": +" << ((scores[i]/minscore - 1)*100) << "%" << endl;

	cout << "Backprop weights:" << endl << weightsets[0] << endl;

	cin.get();

	main();
	return 0;
}
