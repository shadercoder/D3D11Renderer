#pragma once
#include "SampleFramework/SampleBase.h"
#include "Clair/Scene.h"
#include "Clair/MaterialInstance.h"
#include <Clair/RenderTargetGroup.h>

class DeferredSample : public SampleFramework::SampleBase {
public:
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize(int width, int height, float aspect) override;
	void update() override;
	void render() override;

private:
	void createRenderTarget(Clair::RenderTarget*& outRenderTarget,
							Clair::Texture*& outTexture) const;

	Clair::Scene* mScene {nullptr};
	Clair::MaterialInstance* mDeferredCompositeMat {nullptr};
	class Cb_materials_deferredGeometry_Ps* mConstBuffer {nullptr};

	Clair::RenderTarget* mGBufAlbedo {nullptr};
	Clair::Texture* mGBufAlbedoTex {nullptr};
	Clair::RenderTarget* mGBufNormal {nullptr};
	Clair::Texture* mGBufNormalTex {nullptr};
	Clair::RenderTarget* mGBufPosition {nullptr};
	Clair::Texture* mGBufPositionTex {nullptr};
	Clair::DepthStencilTarget* mGBufDepthStencil {nullptr};
	Clair::RenderTargetGroup* mGBuffer {nullptr};
};