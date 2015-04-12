#pragma once
#include "SampleFramework/SampleBase.h"
#include "Clair/Scene.h"
#include "Clair/Matrix.h"

class BasicSample : public SampleFramework::SampleBase {
public:
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize(int width, int height, float aspect) override;
	void update(float deltaTime, float runningTime) override;
	void render() override;

private:
	Clair::Scene* mScene {nullptr};
	Clair::Float4x4 mPerspective {};
};
