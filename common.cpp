#include "common.h"

double rand11(){
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<double> d(-1.0, 1.0);
	return d(gen);
}
double rand01(){
	return (rand11() + 1) / 2;
}
int randInt(int a, int b){ // Inclusive on both ends.
	return (int)floor(a + (b - a + 1) * rand01());
}

//http://en.cppreference.com/w/cpp/chrono/c/strftime
//https://stackoverflow.com/questions/14386923/localtime-vs-localtime-s-and-appropriate-input-arguments
std::string startTimestamp(time_t &startTime){
	std::string out = "";
	startTime = std::time(NULL);
	char timeFormatStr[100];
	struct tm timeinfo;
	localtime_s(&timeinfo, &startTime);
	if (std::strftime(timeFormatStr, sizeof(timeFormatStr), "%b %d, %Y %H:%M:%S", &timeinfo))
		out += "Started execution: " + (std::string)timeFormatStr + "\n\n";
	return out;
}
std::string endTimestamp(time_t startTime){
	std::string out = "";
	time_t endTime = std::time(NULL);
	char timeFormatStr[100];
	struct tm timeinfo;
	localtime_s(&timeinfo, &endTime);
	if (std::strftime(timeFormatStr, sizeof(timeFormatStr), "%b %d, %Y %H:%M:%S", &timeinfo))
		out += "\n\nCompleted execution: " + (std::string)timeFormatStr +"\n";
	out += "Total execution time: " + std::to_string(endTime - startTime) + " seconds\n\n\n\n\n";
	return out;
}

