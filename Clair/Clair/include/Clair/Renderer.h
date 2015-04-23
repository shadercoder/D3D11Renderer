#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"
#include "Clair/Texture.h"

struct HWND__;
typedef HWND__* HWND;

namespace Clair {
	class Scene;
	class MaterialInstance;
	class RenderTargetGroup;

	class Renderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static void setViewport(int x, int y, int width, int height);
		static void clear(bool clearCol);
		static void finalizeFrame();

		static void setRenderTargetGroup(const RenderTargetGroup* targets);
		static void setRenderPass(RenderPass pass);
		static void setViewMatrix(const Float4x4& view);
		static void setProjectionMatrix(const Float4x4& projection);
		static void setCameraPosition(const Float3& position);
		static void render(Scene* scene);
		static void renderScreenQuad(const MaterialInstance* materialInstance);
		static RenderTargetGroup* getDefaultRenderTargetGroup();

	private:
		static RenderTargetGroup* msDefaultRenderTargetGroup;
	};

	inline RenderTargetGroup* Renderer::getDefaultRenderTargetGroup() {
		return msDefaultRenderTargetGroup;
	}
}
