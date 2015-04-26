#pragma once
#include "SampleFramework/SampleBase.h"
#include "Clair/Scene.h"
#include "Clair/MaterialInstance.h"
#include <Clair/RenderTargetGroup.h>
#include <array>

class DeferredSample : public SampleFramework::SampleBase {
public:
	static const int NUM_LIGHTS = 128;
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize(int width, int height, float aspect) override;
	void update() override;
	void render() override;

private:
	void createRenderTarget(Clair::RenderTarget*& outRenderTarget,
							Clair::Texture*& outTexture) const;
	void resetLights();

	Clair::Scene* mScene {nullptr};
	Clair::MaterialInstance* mDeferredCompositeMat {nullptr};
	class Cb_materials_deferredGeometry_Ps* mGeometryCBuffer {nullptr};
	class Cb_materials_deferredComposite_Ps* mCompositeCBuffer {nullptr};

	Clair::RenderTarget* mGBufAlbedo {nullptr};
	Clair::Texture* mGBufAlbedoTex {nullptr};
	Clair::RenderTarget* mGBufNormal {nullptr};
	Clair::Texture* mGBufNormalTex {nullptr};
	Clair::RenderTarget* mGBufPosition {nullptr};
	Clair::Texture* mGBufPositionTex {nullptr};
	Clair::DepthStencilTarget* mGBufDepthStencil {nullptr};
	Clair::RenderTargetGroup* mGBuffer {nullptr};

	class Light {
	public:
		Clair::Float3 color {};
		float intensity {1.0f};
		float height {0.0f};
		float rotationRadius {0.0f};
		float rotationSpeed {1.0f};
		float offset {0.0f};
	};
	std::array<Light, NUM_LIGHTS> mLights;
};