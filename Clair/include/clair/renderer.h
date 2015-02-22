#pragma once
#include <string>

namespace Clair {
	class Scene;
	class Matrix;

	class Renderer {
	public:
		static bool initialize(HWND hwnd, const std::string& clairDataPath);
		static void terminate();

		static void setViewport(float x, float y, float width, float height);
		static void clear();
		static void finalizeFrame();
		static void render(Scene* scene);

		static Scene* createScene();

		static void setCameraMatrix(const Clair::Matrix& m);
	};
};
