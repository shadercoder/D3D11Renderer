#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"

namespace Clair {
	class Scene;
	class MaterialInstance;

	class Renderer {
	public:
		static bool initialize();
		static void terminate();

		static void setViewport(int x, int y, int width, int height);
		static void clear(bool clearCol);
		static void finalizeFrame();

		static void setRenderPass(RenderPass pass);
		static void setViewMatrix(const Float4x4& view);
		static void setProjectionMatrix(const Float4x4& projection);
		static void setCameraPosition(const Float3& position);
		static void render(Scene* scene);
		static void renderScreenQuad(const MaterialInstance* materialInstance);
	};
};
