#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"

struct ID3D11Device;
struct HWND__;
typedef HWND__* HWND;

namespace Clair {
	class Scene;
	class MaterialInstance;

	class LowLevelRenderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static ID3D11Device* getD3dDevice();
		static void clear(bool clearCol);
		static void finalizeFrame();
		static void setViewport(int x, int y, int width, int height);
		static void render(Scene* scene);
		static void setViewMatrix(const Float4x4& view);
		static void setProjectionMatrix(const Float4x4& projection);
		static void setRenderPass(RenderPass pass);
		static void setCameraPosition(const Float3& position);
		static void renderScreenQuad(const MaterialInstance* materialInstance);
	};
}