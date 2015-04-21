#pragma once
#include <iostream>
#include "SampleFramework/Timer.h"

namespace SampleFramework {
	class Logger {
	public:
		static void log(const std::string& message);

	private:
		static void printTime();
		static void reset(double runningTime);
		static double msRunningTime;
		static Timer msTimer;
		friend class Framework;
	};
}