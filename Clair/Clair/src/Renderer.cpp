#include <Windows.h>
#include "Clair/Renderer.h"
#include <stdio.h>
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include <vector>
#include "Clair/Matrix.h"
#include "Material.h"
#include "Mesh.h"
#include "Serialization.h"
#include "LowLevelRenderer.h"
#include <cassert>

using namespace Clair;

namespace {
	std::vector<Clair::Scene*> scenes;
	std::vector<Clair::Mesh*> meshes;
	std::vector<Clair::Material*> materials;
}

bool Clair::Renderer::initialize(const HWND hwnd) {
	const bool result {LowLevelRenderer::initialize(hwnd)};
	printf("Clair initialized.\n");
	return result;
}

void Clair::Renderer::terminate() {
	for (const auto& it : scenes) {
		delete it;
	}
	for (const auto& it : meshes) {
		delete it->vertexBuffer;
		delete it->indexBuffer;
		delete it;
	}
	for (const auto& it : materials) {
		delete it;
	}
	LowLevelRenderer::terminate();
	printf("Clair terminated.\n");
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

Mesh* Renderer::createMesh(const char* data) {
	assert(data);
	VertexLayout vertexLayout {Serialization::readVertexLayoutFromBytes(data)};
	unsigned stride {0};
	memcpy(&stride, data, sizeof(unsigned));
	data += sizeof(unsigned);
	unsigned numVertices {0};
	memcpy(&numVertices, data ,sizeof(unsigned));
	data += sizeof(unsigned);
	const char* const vertexData {data};
	data += sizeof(char) * numVertices * stride;
	unsigned numIndices {0};
	memcpy(&numIndices, data, sizeof(unsigned));
	data += sizeof(unsigned);
	const unsigned* const indexData {reinterpret_cast<const unsigned*>(data)};

	Mesh* const mesh {new Mesh{}};
	mesh->vertexLayout = vertexLayout;
	mesh->vertexBuffer = new VertexBuffer {LowLevelRenderer::getD3dDevice(),
										   vertexData, numVertices * stride};
	mesh->indexBuffer = new IndexBuffer {
		LowLevelRenderer::getD3dDevice(), indexData,
		numIndices * sizeof(unsigned)
	};
	mesh->indexBufferSize = numIndices;
	meshes.push_back(mesh);
	return mesh;
}

Material* Renderer::createMaterial(char* const data) {
	Material* const material {
		new Material {LowLevelRenderer::getD3dDevice(), data}
	};
	materials.push_back(material);
	return material;
}

void Clair::Renderer::setCameraMatrix(const Clair::Matrix& m) {
	LowLevelRenderer::setCameraMatrix(m);
	return;
}
