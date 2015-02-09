#pragma once
#include <random>
#include <string>
#include <ctime>

double rand11();
double rand01();
int randInt(int a, int b);

std::string startTimestamp(time_t &startTime);
std::string endTimestamp(time_t startTime);
