#pragma once
#include <string>

namespace Clair {
	class Scene;

	class Renderer {
	public:
		static bool initialize(HWND hwnd, const std::string& clairDataPath);
		static void terminate();

		static void setViewport(float x, float y, float width, float height);
		static void render();

		static Scene* createScene();
	};
};
