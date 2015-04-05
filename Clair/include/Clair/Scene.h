#pragma once
#include <vector>

namespace Clair {
	class Renderer;
	class Object;

	class Scene {
	public:
		Scene();
		~Scene();

		Object* createObject();

	private:
		friend class Renderer;
		friend class LowLevelRenderer;
		std::vector<Object*> mObjects;
	};
};
