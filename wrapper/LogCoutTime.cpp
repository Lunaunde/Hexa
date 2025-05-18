#include "LogCoutTime.h"
#include <iostream>
#include <ctime>
#include <chrono>
void LCT()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm* local_time = std::localtime(&now_time);
	std::cout << local_time->tm_year + 1900 << '/' << local_time->tm_mon << '/' << local_time->tm_mday << " " << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec << " ";
}