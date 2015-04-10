#pragma once

struct HWND__;
typedef HWND__* HWND;

namespace SampleFramework {
	class SampleBase {
	public:
		SampleBase() = default;
		virtual ~SampleBase() = default;

		virtual bool initialize(HWND hwnd) = 0;
		virtual void terminate() = 0;

		virtual void update(float deltaTime, float runningTime) = 0;
		virtual void render() = 0;
		virtual void onResize(int width, int height) = 0;
	};
}