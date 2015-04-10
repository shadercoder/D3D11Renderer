#pragma once
#include <string>

namespace SampleFramework {
	class SampleBase;

	class Framework {
	public:
		static bool run(SampleBase* sample, const std::string& caption);
	};
}