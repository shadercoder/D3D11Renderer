#pragma once
#include "SampleFramework/SampleBase.h"
#include "Clair/Scene.h"
#include "Clair/MaterialInstance.h"
#include "Clair/RenderTargetGroup.h"
#include "SampleFramework/GlmMath.h"

class AdvancedSample : public SampleFramework::SampleBase {
public:
	~AdvancedSample() override;
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize() override;
	void update() override;
	void render() override;

private:
	void filterCubeMap();
	void loadAndSetNewCubeMap(const std::string& filename);
	void filterReflectionBuffer();
	void resetLights();
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
	class Cb_materials_pbrSky_Ps* mSkyConstBuffer {nullptr};

	Clair::Texture* RT0 {nullptr};
	Clair::Texture* RT1 {nullptr};
	Clair::Texture* RT2 {nullptr};
	Clair::Texture* RT3 {nullptr};
	Clair::RenderTargetGroup* mGBuffer {nullptr};
	Clair::MaterialInstance* mCompositeMat {nullptr};
	class Cb_materials_advanced_composite_Ps* mCompositeCBuffer {nullptr};

	Clair::Texture* mReflectionTex {nullptr};
	Clair::RenderTargetGroup* mReflectionBuffer {nullptr};
	Clair::MaterialInstance* mReflectionMatInstance {nullptr};
	Clair::MaterialInstance* mFilterReflectionMatInstance {nullptr};
	Clair::MaterialInstance* mDrawTextureMatInstance {nullptr};
	class Cb_materials_advanced_ssr_Ps* mReflectionCbuffer {nullptr};
	Clair::Texture* mPostProcessTex[2];
	Clair::RenderTargetGroup* mPostProcessBuffer[2];

	float mFoV {80.0f};
	float mGlossiness {0.8f};
	float mMetalness {0.8f};
	class Cb_materials_advanced_geometry_Ps* mTweakableCbuf {nullptr};

	bool mSSREnabled {true};
	int mEnvironmentMap {0};
	Clair::Texture* mHdrSkyTex {nullptr};
	float mAmbient {0.8f};
	
	bool mDrawLightDebugCubes {true};
	bool mLightsEnabled {false};
	Clair::Scene* mLightDebugScene {nullptr};
	class Light {
	public:
		glm::vec3 color {};
		Clair::Object* debugObj {nullptr};
		float intensity {1.0f};
		float height {0.0f};
		float rotationRadius {0.0f};
		float rotationSpeed {1.0f};
		float offset {0.0f};
	};
	Light* mLights {nullptr};
};