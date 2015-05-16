#pragma once
#include "SampleFramework/SampleBase.h"
#include "Clair/Scene.h"
#include "Clair/MaterialInstance.h"
#include "../../data/materials/pbr/pbrSky.h"
#include "Clair/RenderTargetGroup.h"
#include "SampleFramework/GlmMath.h"

class IBLSample : public SampleFramework::SampleBase {
public:
	~IBLSample() override;
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize() override;
	void update() override;
	void render() override;

private:
	void filterCubeMap();
	Clair::Texture* createGBufferTarget(
		Clair::Texture::Format format,
		Clair::Texture::Type type) const;
	glm::mat4 mProjectionMat;
	Clair::Scene* mScene {nullptr};

	Clair::Texture* mSkyTexture {nullptr};
	Clair::MaterialInstance* mPanoramaToCube {nullptr};
	Clair::MaterialInstance* mSkyMaterialInstance {nullptr};
	Clair::MaterialInstance* mFilterCubeMapMatInstance {nullptr};
	class Cb_materials_filterCube_Ps* mFilterCubeMapCBuffer {nullptr};
	Cb_materials_pbr_pbrSky_Ps* mSkyConstBuffer {nullptr};

	Clair::Texture* RT0 {nullptr};
	Clair::Texture* RT1 {nullptr};
	Clair::Texture* RT2 {nullptr};
	Clair::Texture* RT3 {nullptr};
	Clair::RenderTargetGroup* mGBuffer {nullptr};
	Clair::MaterialInstance* mCompositeMat {nullptr};
	class Cb_materials_pbr_pbrComposite_Ps* mCompositeCBuffer {nullptr};

	float mFoV {60.0f};
	float mGlossiness {1.0f};
	float mMetalness {1.0f};
	class Cb_materials_pbr_pbrGeometry_Ps* mTweakableCbuf {nullptr};
};
