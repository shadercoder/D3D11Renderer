#include "Clair/ResourceManager.h"
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include "Clair/Matrix.h"
#include "Clair/Material.h"
#include "Clair/Texture.h"
#include "Mesh.h"

using namespace Clair;

namespace {
	std::vector<Scene*> scenes;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	std::vector<Texture*> textures;
	std::vector<MaterialInstance*> matInstances;
}

bool ResourceManager::initialize() {
	return true;
}

void ResourceManager::terminate() {
	for (const auto& it : scenes) {
		delete it;
	}
	for (const auto& it : meshes) {
		delete it;
	}
	for (const auto& it : materials) {
		delete it;
	}
	for (const auto& it : textures) {
		delete it;
	}
	for (const auto& it : matInstances) {
		delete it;
	}
}


Scene* ResourceManager::createScene() {
	Scene* const newScene {new Scene{}};
	scenes.push_back(newScene);
	CLAIR_DEBUG_LOG("Scene created successfully");
	return newScene;
}

Mesh* ResourceManager::createMesh(const Byte* const data) {
	CLAIR_ASSERT(data, "Mesh data is null");
	Mesh* const mesh {new Mesh {data}};
	CLAIR_DEBUG_LOG_IF(mesh->isValid(), "Mesh created successfully");
	meshes.push_back(mesh);
	return mesh;
}

Material* ResourceManager::createMaterial(const Byte* const data) {
	CLAIR_ASSERT(data, "Material data is null");
	Material* const material {new Material {data}};
	CLAIR_DEBUG_LOG_IF(material->isValid(), "Material created successfully");
	materials.push_back(material);
	return material;
}

Texture* ResourceManager::createTexture() {
	Texture* const texture {new Texture{}};
	//CLAIR_DEBUG_LOG_IF(texture->isValid(), "Texture created successfully");
	textures.push_back(texture);
	return texture;
}

MaterialInstance*
ResourceManager::createMaterialInstance(const Material* const material) {
	CLAIR_ASSERT(material, "Material should not be null");
	MaterialInstance* newInst {new MaterialInstance{material}};
	CLAIR_DEBUG_LOG_IF(newInst->isValid(), "Texture created successfully");
	matInstances.push_back(newInst);
	return newInst;
}
