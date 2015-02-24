#pragma once
#include <random>
#include <string>
#include <ctime>
#include <cassert>

double rand11();
double rand01();
int randInt(int a, int b);

std::string startTimestamp(time_t &startTime);
std::string endTimestamp(time_t startTime);



#define _WIN32_WINNT 0x0500
#include <windows.h>

void resizeWindow(size_t xpx, size_t ypx);//http://www.cplusplus.com/forum/beginner/1481/
void setConsoleXY(size_t column, size_t line);//http://stackoverflow.com/a/22685071/1181387
void setConsoleCoord(COORD coord);
COORD getConsoleCoord();