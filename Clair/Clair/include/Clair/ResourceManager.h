#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"
#include "Clair/Debug.h"
#include <vector>

namespace Clair {
	class ResourceManager {
	public:
		static bool initialize();
		static void terminate();
		
		static class Scene* createScene();
		static class Mesh* createMesh();
		static class Material* createMaterial();
		static class Texture* createTexture();
		static class MaterialInstance* createMaterialInstance();

	private:
		static std::vector<Scene*> msSceneVec;
		static std::vector<Mesh*> msMeshVec;
		static std::vector<Material*> msMaterialVec;
		static std::vector<Texture*> msTextureVec;
		static std::vector<MaterialInstance*> msMaterialInstanceVec;
	};
};