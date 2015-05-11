#pragma once
#include "SampleFramework/SampleBase.h"
#include "Clair/Scene.h"
#include "Clair/Matrix.h"
#include <Clair/MaterialInstance.h>
#include "../../data/materials/sky.h"

class HDRSample : public SampleFramework::SampleBase {
public:
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize() override;
	void update() override;
	void render() override;

private:
	void filterCubeMap();
	Clair::Scene* mScene {nullptr};

	Clair::Texture* mSkyTexture {nullptr};
	Clair::MaterialInstance* mPanoramaToCube {nullptr};
	Clair::MaterialInstance* mSkyMaterialInstance {nullptr};
	Cb_materials_sky_Ps* mSkyConstBuffer {nullptr};

	Clair::MaterialInstance* mFilterCubeMapMatInstance {nullptr};
	class Cb_materials_filterCube_Ps* mFilterCubeMapCBuffer {nullptr};
	class Cb_materials_pbrSimple_Ps* mModelCBuffer {nullptr};

	Clair::Texture* mSceneTarget {nullptr};

	float mFoV {60.0f};
	float mReflectivity {0.0f};
	float mRoughness {1.0f};
	float mMetalness {0.0f};
};