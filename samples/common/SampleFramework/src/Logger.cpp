#include "SampleFramework/Logger.h"

using namespace SampleFramework;

double Logger::msRunningTime {0.0};
Timer Logger::msTimer {};

void Logger::log(const std::string& message) {
	printTime();
	std::cout << message.c_str() << std::endl;
}

void Logger::printTime() {
	const double totalTime = msRunningTime + msTimer.elapsed();
	int hours = static_cast<int>(totalTime / 3600.0)/* % 100*/;
	int minutes = static_cast<int>(totalTime / 60.0) % 60;
	int seconds = static_cast<int>(totalTime) % 60;
	int ms = static_cast<int>(totalTime * 1000.0) % 1000;
	printf("> %.2i:%.2i:%.2i:%.3i | ", hours, minutes, seconds, ms);
}

void Logger::reset(const double runningTime) {
	msTimer.start();
	msRunningTime = runningTime;
}