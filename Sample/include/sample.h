#pragma once

class Sample {
public:
	void initialize();
	void terminate();

	void update();
	void render();
	void onResize(float width, float height);
};