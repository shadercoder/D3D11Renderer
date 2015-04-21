#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"
#include "Clair/Debug.h"

typedef unsigned char Byte;

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
		static Mesh* createMesh(const Byte* data);
		static Material* createMaterial(const Byte* data);
		static Texture* createTexture();
		static MaterialInstance*
		createMaterialInstance(const Material* material);
	};
};