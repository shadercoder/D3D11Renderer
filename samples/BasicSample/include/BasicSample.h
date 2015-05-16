#pragma once
#include "SampleFramework/SampleBase.h"
#include "Clair/Scene.h"
#include "Clair/Matrix.h"

class BasicSample : public SampleFramework::SampleBase {
public:
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize() override;
	void update() override;
	void render() override;

private:
	Clair::Scene* mScene {nullptr};
	Clair::Object* mBunny {nullptr};
	class Cb_materials_basic_Ps* mConstBuffer {nullptr};
};
