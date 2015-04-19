#pragma once
#include "SampleFramework/SampleBase.h"
#include "Clair/Scene.h"
#include "Clair/Matrix.h"
#include <Clair/MaterialInstance.h>

class MaterialSample : public SampleFramework::SampleBase {
public:
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize(int width, int height, float aspect) override;
	void update() override;
	void render() override;

private:
	Clair::Scene* mScene {nullptr};
	Clair::MaterialInstance* mSkyMaterialInstance;
};
