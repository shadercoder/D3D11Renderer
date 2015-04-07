#pragma once
#include <chrono>

namespace SampleCommon {
	class Timer {
	public:
		void start() {
			startTime = std::chrono::high_resolution_clock::now();
		}

		double elapsed() {
			endTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsedTime = endTime - startTime;
			return elapsedTime.count();
		}
		
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
	};
}