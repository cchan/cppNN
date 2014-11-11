#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>

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
	out << "[" << a[0];
	for (int i = 1; i < a.size(); i++)
		out << "," << a[i];
	out << "]";
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

	tanknums_double.push_back(1);//Bias term

	return tanknums_double;
}

int main(){
	const int actualpop = 1000;
	const int sample = 7;
	const int backprop_rounds = 100000;
	const int score_rounds = 10000;

	static random_device rd;
	static mt19937 gen(rd());
	static uniform_real_distribution<double> preweight(-0.001, 0.001);

	vector<vector<double> > weightsets;
	vector<double> weightable (sample + 1);
		for (int i = 0; i < sample+1; i++) weightable.push_back(preweight(gen));
	vector < double > weights871 (sample + 1, 0);
		weights871[sample - 1] = (sample+1.0) / sample;//Multiply maximum by a factor to extend it a little bit.
		weights871[sample] = -1;//Bias is -1
	
	weightsets.push_back(weightable);
	weightsets.push_back(weights871);

	vector<double> scores(weightsets.size());

	for (unsigned long long n = 0; true; n++){
		vector<double> tanks = sampleTanks(actualpop, sample);

		if (n < backprop_rounds){ //Backpropagate for ? rounds
			float learningrate = 1.0 / (40000 * (n + 1000));
			weightsets[0] -= tanks * (weightsets[0] * tanks - 1000) * learningrate;
			if (n % 1000 == 0) cout << n << " diff: " << (weightsets[0] * tanks - 1000) << endl;
		}
		else if (n < backprop_rounds + score_rounds) {//Score for ? rounds
			for (int i = 0; i < scores.size(); i++)
				scores[i] += abs(weightsets[i] * tanks - 1000);
		}
		else
			break;
	}

	cout << endl << 100*(1 - scores[0] / scores[1]) << "% better than max871" << endl << weightsets[0];
	cin.get();

	return 0;
}
