#pragma once
#include <Windows.h>

class Sample {
public:
	bool initialize(HWND hwnd);
	void terminate();

	void update(float deltaTime, float runningTime);
	void render();
	void onResize(int width, int height);
};
