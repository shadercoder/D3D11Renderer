#pragma once

namespace SampleFramework {
	class Random {
	public:
		static void initialize();
		static float randomFloat(float min = 0.0f, float max = 1.0f);
	};
}