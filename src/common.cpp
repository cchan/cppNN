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
	assert(b >= a);
	return (int)floor(a + (b - a + 1) * rand01());
}

//http://en.cppreference.com/w/cpp/chrono/c/strftime
//https://stackoverflow.com/questions/14386923/localtime-vs-localtime-s-and-appropriate-input-arguments
std::string startTimestamp(time_t &startTime){
	std::string out = "";
	startTime = std::time(NULL);
	char timeFormatStr[100];
	struct tm *timeinfo;
	timeinfo = localtime(&startTime);
	if (std::strftime(timeFormatStr, sizeof(timeFormatStr), "%b %d, %Y %H:%M:%S", timeinfo))
		out += "Started execution: " + (std::string)timeFormatStr + "\n\n";
	return out;
}
std::string endTimestamp(time_t startTime){
	std::string out = "";
	time_t endTime = std::time(NULL);
	char timeFormatStr[100];
	struct tm *timeinfo;
	timeinfo = localtime(&endTime);
	if (std::strftime(timeFormatStr, sizeof(timeFormatStr), "%b %d, %Y %H:%M:%S", timeinfo))
		out += "\n\nCompleted execution: " + (std::string)timeFormatStr +"\n";
	out += "Total execution time: " + std::to_string(endTime - startTime) + " seconds\n\n\n\n\n";
	return out;
}






void resizeWindow(size_t xpx, size_t ypx){
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions

	MoveWindow(console, r.left, r.top, xpx, ypx, TRUE);
}


void setConsoleXY(size_t column, size_t line)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	assert(SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
		));
}
void setConsoleCoord(COORD coord)
{
	assert(SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
		));
}
COORD getConsoleCoord(){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	assert(GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&csbi
		));
	return csbi.dwCursorPosition;
}
