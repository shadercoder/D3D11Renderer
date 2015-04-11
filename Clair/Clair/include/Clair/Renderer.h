#pragma once
#include "Clair/RenderPass.h"

struct HWND__;
typedef HWND__* HWND;

namespace Clair {
	class Scene;
	class Matrix;
	class Mesh;
	class Material;

	class Renderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static void setViewport(int x, int y, int width, int height);
		static void clear();
		static void finalizeFrame();

		static void setRenderPass(RenderPass pass);
		static Scene* createScene();
		static Mesh* createMesh(const char* data);
		static Material* createMaterial(char* data);
		static void setCameraMatrix(const Clair::Matrix& m);
		static void render(Scene* scene);
	};
};
