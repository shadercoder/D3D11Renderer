#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"

struct ID3D11Device;
struct HWND__;
typedef HWND__* HWND;

namespace Clair {
	class Scene;

	class LowLevelRenderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static ID3D11Device* getD3dDevice();
		static void clear();
		static void finalizeFrame();
		static void setViewport(int x, int y, int width, int height);
		static void render(Scene* scene);
		static void setCameraMatrix(const Float4x4& m);
		static void setRenderPass(RenderPass pass);
	};
}