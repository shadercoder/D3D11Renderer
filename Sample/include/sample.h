#pragma once

class Sample {
public:
	bool initialize(HWND hwnd);
	void terminate();

	void update(float deltaTime, float runningTime);
	void render();
	void onResize(float width, float height);
};