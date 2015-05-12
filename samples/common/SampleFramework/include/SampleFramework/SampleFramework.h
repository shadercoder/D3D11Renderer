#pragma once
#include "SampleFramework/Logger.h"

namespace SampleFramework {
	class SampleBase;

	class Framework {
	public:
		static int run(SampleBase* sample, const std::string& caption,
					   int width, int height,
					   const std::string& dataPath);
	private:
		static void setWindowSize(int width, int height);
		static void resizeSample();
		static void handleEvents();
	};
}