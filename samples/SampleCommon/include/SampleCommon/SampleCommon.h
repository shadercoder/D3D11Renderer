#pragma once
#include <string>

namespace SampleCommon {
	class SampleBase;

	class Framework {
	public:
		static bool run(SampleBase* sample, const std::string& caption);
	};
}