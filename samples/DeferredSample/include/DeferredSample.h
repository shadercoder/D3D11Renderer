#pragma once
#include "SampleFramework/SampleBase.h"
#include <array>
#include "Clair/Scene.h"
#include "Clair/MaterialInstance.h"
#include "Clair/RenderTargetGroup.h"
#include "Clair/Mesh.h"
#include "SampleFramework/GlmMath.h"
#include "../../data/materials/NumLights.h"


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
	void createObject(Clair::Mesh* mesh, const Clair::Float4& color,
					  const Clair::Float4x4& transform);
	void resetLights();

	Clair::Scene* mScene {nullptr};
	Clair::Scene* mLightDebugScene {nullptr};
	Clair::Material* mGeometryMat {nullptr};
	Clair::MaterialInstance* mDeferredCompositeMat {nullptr};
	class Cb_materials_deferredGeometry_Ps* mGeometryCBuffer {nullptr};
	class Cb_materials_deferredComposite_Ps* mCompositeCBuffer {nullptr};
	bool mDrawLightDebugCubes {true};

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
		glm::vec3 color {};
		Clair::Object* debugObj {nullptr};
		float intensity {1.0f};
		float height {0.0f};
		float rotationRadius {0.0f};
		float rotationSpeed {1.0f};
		float offset {0.0f};
	};
	std::array<Light, NUM_LIGHTS> mLights;
};