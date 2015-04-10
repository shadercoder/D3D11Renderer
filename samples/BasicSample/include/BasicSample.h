#pragma once
#include "SampleFramework/SampleBase.h"

class BasicSample : public SampleFramework::SampleBase {
public:
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void update(float deltaTime, float runningTime) override;
	void render() override;
	void onResize(int width, int height) override;
};
