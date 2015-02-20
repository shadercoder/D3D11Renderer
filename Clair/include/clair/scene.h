#pragma once
#include <vector>

namespace Clair {
	class Renderer;
	class Object;
	class Camera;

	class Scene {
	public:
		Scene();
		~Scene();

		Object* createObject();
		Camera* createCamera();

	private:
		friend class Renderer;
		std::vector<Object*> mObjects;
		std::vector<Camera*> mCameras;
	};
};
