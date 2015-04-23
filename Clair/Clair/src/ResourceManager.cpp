#include "Clair/ResourceManager.h"
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include "Clair/Matrix.h"
#include "Clair/Material.h"
#include "Clair/Texture.h"
#include "Clair/Mesh.h"
#include "Clair/RenderTarget.h"
#include "Clair/DepthStencilTarget.h"

using namespace Clair;

#define CLAIR_RESOURCE_MANAGER_CREATABLE_TYPE(T)\
std::vector<T*> ResourceManager::ms##T##Vec;\
\
T* ResourceManager::create##T() {\
	T* const newT {new T{}};\
	CLAIR_DEBUG_LOG(#T" created successfully");\
	ms##T##Vec.push_back(newT);\
	return newT;\
}

CLAIR_RESOURCE_MANAGER_CREATABLE_TYPE(Scene);
CLAIR_RESOURCE_MANAGER_CREATABLE_TYPE(Mesh);
CLAIR_RESOURCE_MANAGER_CREATABLE_TYPE(Material);
CLAIR_RESOURCE_MANAGER_CREATABLE_TYPE(Texture);
CLAIR_RESOURCE_MANAGER_CREATABLE_TYPE(MaterialInstance);
CLAIR_RESOURCE_MANAGER_CREATABLE_TYPE(RenderTarget);
CLAIR_RESOURCE_MANAGER_CREATABLE_TYPE(DepthStencilTarget);

bool ResourceManager::initialize() {
	return true;
}

void ResourceManager::terminate() {
	for (const auto& it : msSceneVec) {
		delete it;
	}
	for (const auto& it : msMeshVec) {
		delete it;
	}
	for (const auto& it : msMaterialVec) {
		delete it;
	}
	for (const auto& it : msTextureVec) {
		delete it;
	}
	for (const auto& it : msMaterialInstanceVec) {
		delete it;
	}
	for (const auto& it : msRenderTargetVec) {
		delete it;
	}
	for (const auto& it : msDepthStencilTargetVec) {
		delete it;
	}
}