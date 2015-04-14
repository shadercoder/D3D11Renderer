#pragma once
#include <iostream>

namespace SampleFramework {
	class Logger {
	public:
		static inline void logCallback(const std::string& message) {
			std::cout << message.c_str() << std::endl;
		}
	};
}