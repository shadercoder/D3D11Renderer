#include "Clair/ResourceManager.h"
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include "Clair/Matrix.h"
#include "Clair/Material.h"
#include "Clair/Texture.h"
#include "Clair/Mesh.h"

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
}