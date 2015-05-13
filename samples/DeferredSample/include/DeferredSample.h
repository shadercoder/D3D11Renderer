#pragma once
#include "SampleFramework/SampleBase.h"
#include <array>
#include "Clair/Scene.h"
#include "Clair/MaterialInstance.h"
#include "Clair/RenderTargetGroup.h"
#include "Clair/Mesh.h"
#include "SampleFramework/GlmMath.h"
#include "../../rawdata/materials/numLights.h"


class DeferredSample : public SampleFramework::SampleBase {
public:
	bool initialize(HWND hwnd) override;
	void terminate() override;

	void onResize() override;
	void update() override;
	void render() override;

private:
	Clair::Texture* createGBufferTarget(
		Clair::Texture::Format format,
		Clair::Texture::Type type) const;
	void createObject(Clair::Mesh* mesh, const Clair::Float3& color,
					  const Clair::Float4x4& transform);
	void resetLights();

	Clair::Scene* mScene {nullptr};
	Clair::Scene* mLightDebugScene {nullptr};
	Clair::Material* mGeometryMat {nullptr};
	Clair::MaterialInstance* mDeferredCompositeMat {nullptr};
	class Cb_materials_deferredComposite_Ps* mCompositeCBuffer {nullptr};
	bool mDrawLightDebugCubes {true};
	bool mDrawGBuffers {true};

	Clair::Texture* mGBufAlbedo {nullptr};
	Clair::Texture* mGBufNormal {nullptr};
	Clair::Texture* mGBufPosition {nullptr};
	Clair::Texture* mGBufDepthStencil {nullptr};
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
	Light* mLights {nullptr};
};