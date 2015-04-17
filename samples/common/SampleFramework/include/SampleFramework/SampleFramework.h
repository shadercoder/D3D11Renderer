#pragma once
#include "SampleFramework/Logger.h"

namespace SampleFramework {
	class SampleBase;

	class Framework {
	public:
		static bool run(SampleBase* sample, const std::string& caption,
						int width, int height,
						const std::string& dataPath);
	};
}