#pragma once
#include <string>
#include <vector>

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

		static Scene* createScene();
		static Material* createMaterial(char* data);
		static Mesh* createMesh(char* data);
		static void setCameraMatrix(const Clair::Matrix& m);
		static void render(Scene* scene);
	};
};
