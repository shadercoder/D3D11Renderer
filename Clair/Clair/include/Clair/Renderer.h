#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"
#include "Clair/Debug.h"

struct HWND__;
typedef HWND__* HWND;

namespace Clair {
	class Scene;
	class Mesh;
	class Material;
	class Texture;

	class Renderer {
	public:
		static bool initialize(HWND hwnd, LogCallback logCallback);
		static void terminate();

		static void setViewport(int x, int y, int width, int height);
		static void clear();
		static void finalizeFrame();

		static void setRenderPass(RenderPass pass);
		static Scene* createScene();
		static Mesh* createMesh(const char* data);
		static Material* createMaterial(const char* data);
		static Texture* createTexture();
		static void setViewMatrix(const Float4x4& view);
		static void setProjectionMatrix(const Float4x4& projection);
		static void render(Scene* scene);
	};
};
