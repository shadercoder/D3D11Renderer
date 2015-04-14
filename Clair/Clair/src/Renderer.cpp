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

using namespace Clair;

namespace {
	std::vector<Clair::Scene*> scenes;
	std::vector<Clair::Mesh*> meshes;
	std::vector<Clair::Material*> materials;
}

bool Clair::Renderer::initialize(const HWND hwnd, LogCallback logCallback) {
	const bool result {LowLevelRenderer::initialize(hwnd)};
	Log::msCallback = logCallback;
	CLAIR_LOG("Clair initialized");
	return result;
}

void Clair::Renderer::terminate() {
	for (const auto& it : scenes) {
		delete it;
	}
	for (const auto& it : meshes) {
		delete it;
	}
	for (const auto& it : materials) {
		delete it;
	}
	LowLevelRenderer::terminate();
	CLAIR_LOG("Clair terminated");
}

void Clair::Renderer::clear() {
	LowLevelRenderer::clear();
}

void Clair::Renderer::finalizeFrame() {
	LowLevelRenderer::finalizeFrame();
}

void Renderer::setRenderPass(const RenderPass pass) {
	LowLevelRenderer::setRenderPass(pass);
}

void Clair::Renderer::setViewport(const int x, const int y,
								  const int width, const int height) {
	LowLevelRenderer::setViewport(x, y, width, height);
}

void Clair::Renderer::render(Scene* const scene) {
	if (!scene) return;

	LowLevelRenderer::render(scene);
}

Clair::Scene* Clair::Renderer::createScene() {
	Clair::Scene* const newScene {new Scene{}};
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

void Renderer::setViewMatrix(const Float4x4& view) {
	LowLevelRenderer::setViewMatrix(view);
}

void Renderer::setProjectionMatrix(const Float4x4& projection) {
	LowLevelRenderer::setProjectionMatrix(projection);
}
