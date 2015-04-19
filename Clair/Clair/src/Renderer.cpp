#include <Windows.h>
#include "Clair/Renderer.h"
#include <stdio.h>
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include <vector>
#include "Clair/Matrix.h"
#include "Clair/Material.h"
#include "Mesh.h"
#include "LowLevelRenderer.h"
#include "Clair/Debug.h"
#include "Clair/Texture.h"

using namespace Clair;

namespace {
	std::vector<Scene*> scenes;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	std::vector<Texture*> textures;
	std::vector<MaterialInstance*> matInstances;
}

bool Renderer::initialize(const HWND hwnd, LogCallback logCallback) {
	const bool result {LowLevelRenderer::initialize(hwnd)};
	Log::msCallback = logCallback;
	CLAIR_DEBUG_LOG("Clair initialized");
	return result;
}

void Renderer::terminate() {
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
	LowLevelRenderer::terminate();
	CLAIR_DEBUG_LOG("Clair terminated");
}

void Renderer::clear() {
	LowLevelRenderer::clear();
}

void Renderer::finalizeFrame() {
	LowLevelRenderer::finalizeFrame();
}

void Renderer::setRenderPass(const RenderPass pass) {
	LowLevelRenderer::setRenderPass(pass);
}

void Renderer::setViewport(const int x, const int y,
								  const int width, const int height) {
	LowLevelRenderer::setViewport(x, y, width, height);
}

Scene* Renderer::createScene() {
	Scene* const newScene {new Scene{}};
	scenes.push_back(newScene);
	return newScene;
}

Mesh* Renderer::createMesh(const char* const data) {
	CLAIR_ASSERT(data, "Mesh data is null");
	Mesh* const mesh {new Mesh {data}};
	meshes.push_back(mesh);
	return mesh;
}

Material* Renderer::createMaterial(const char* const data) {
	CLAIR_ASSERT(data, "Material data is null");
	Material* const material {new Material {data}};
	materials.push_back(material);
	return material;
}

Texture* Renderer::createTexture(const char* const data) {
	CLAIR_ASSERT(data, "Material data is null");
	Texture* const texture {new Texture{data}};
	textures.push_back(texture);
	return texture;
}

MaterialInstance*
Renderer::createMaterialInstance(const Material* const material) {
	CLAIR_ASSERT(material, "Material should not be null");
	MaterialInstance* newInst {new MaterialInstance{material}};
	matInstances.push_back(newInst);
	return newInst;
}

void Renderer::setViewMatrix(const Float4x4& view) {
	LowLevelRenderer::setViewMatrix(view);
}

void Renderer::setProjectionMatrix(const Float4x4& projection) {
	LowLevelRenderer::setProjectionMatrix(projection);
}

void Renderer::setCameraPosition(const Float3& position) {
	LowLevelRenderer::setCameraPosition(position);
}

void Renderer::render(Scene* const scene) {
	if (!scene) return;

	LowLevelRenderer::render(scene);
}

void Renderer::renderScreenQuad(const MaterialInstance* const materialInstance) {
	LowLevelRenderer::renderScreenQuad(materialInstance);
}
