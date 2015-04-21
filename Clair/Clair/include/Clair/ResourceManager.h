#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"
#include "Clair/Debug.h"

namespace Clair {
	class Scene;
	class Mesh;
	class Material;
	class Texture;
	class MaterialInstance;

	class ResourceManager {
	public:
		static bool initialize();
		static void terminate();

		static Scene* createScene();
		static Mesh* createMesh();
		static Material* createMaterial();
		static Texture* createTexture();
		static MaterialInstance* createMaterialInstance();
	};
};