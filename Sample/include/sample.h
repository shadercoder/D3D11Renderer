#pragma once

class Sample {
public:
	bool initialize(HWND hwnd);
	void update();
	void render();
	void terminate();

	void onResize(float width, float height);
};