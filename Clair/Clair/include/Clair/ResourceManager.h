#pragma once
#include "Clair/RenderPass.h"
#include "Clair/Matrix.h"
#include "Clair/Debug.h"
#include <vector>

namespace Clair {
	class Mesh;
	class Material;
	class RenderTarget;
	class DepthStencilTarget;

	class ResourceManager {
	public:
		static bool initialize();
		static void terminate();
		
		static class Scene* createScene();
		static class Mesh* createMesh();
		static class Material* createMaterial();
		static class Texture* createTexture();
		static class MaterialInstance* createMaterialInstance();
		static class RenderTarget* createRenderTarget();
		static class DepthStencilTarget* createDepthStencilTarget();

	private:
		static std::vector<Scene*> msSceneVec;
		static std::vector<Mesh*> msMeshVec;
		static std::vector<Material*> msMaterialVec;
		static std::vector<Texture*> msTextureVec;
		static std::vector<MaterialInstance*> msMaterialInstanceVec;
		static std::vector<RenderTarget*> msRenderTargetVec;
		static std::vector<DepthStencilTarget*> msDepthStencilTargetVec;
	};
};