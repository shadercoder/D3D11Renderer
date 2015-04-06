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

bool Clair::Renderer::initialize(HWND hwnd) {
	const bool result {LowLevelRenderer::initialize(hwnd)};
	printf("Clair initialized.\n");
	return result;
}

void Clair::Renderer::terminate() {
	LowLevelRenderer::terminate();
	for (const auto& it : scenes) {
		delete it;
	}
	for (const auto& it : meshes) {
		LowLevelRenderer::destroyVertexBuffer(it->vertexBuffer);
		LowLevelRenderer::destroyIndexBuffer(it->indexBuffer);
		delete it;
	}
	for (const auto& it : materials) {
		LowLevelRenderer::destroyVertexShader(it->vertexShader);
		LowLevelRenderer::destroyPixelShader(it->pixelShader);
		delete it;
	}
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
	Clair::Scene* const newScene = new Scene();
	scenes.push_back(newScene);
	return newScene;
}

Mesh* Renderer::createMesh(char* data) {
	assert(data);
	VertexLayout vertexLayout {Serialization::readVertexLayoutFromBytes(data)};
	unsigned stride {0};
	memcpy(&stride, data, sizeof(unsigned));
	data += sizeof(unsigned);
	unsigned numVertices {0};
	memcpy(&numVertices, data ,sizeof(unsigned));
	data += sizeof(unsigned);
	char* const vertexData {data};
	data += sizeof(char) * numVertices * stride;
	unsigned numIndices {0};
	memcpy(&numIndices, data, sizeof(unsigned));
	data += sizeof(unsigned);
	unsigned* const indexData {reinterpret_cast<unsigned*>(data)};

	Mesh* const mesh {new Mesh{}};
	mesh->vertexLayout = vertexLayout;
	mesh->vertexBuffer = LowLevelRenderer::
		createVertexBuffer(vertexData, numVertices * stride);
	mesh->indexBuffer = LowLevelRenderer::
		createIndexBuffer(indexData, numIndices * sizeof(unsigned));
	mesh->indexBufferSize = numIndices;
	meshes.push_back(mesh);
	return mesh;
}

Material* Renderer::createMaterial(char* data) {
	assert(data);
	const VertexLayout vertexLayout {
		Serialization::readVertexLayoutFromBytes(data)};
	size_t vsSize {0};
	memcpy(&vsSize, data, sizeof(size_t));
	data += sizeof(size_t);
	char* const vsData = data;
	data += sizeof(char) * vsSize;
	size_t psSize {0};
	memcpy(&psSize, data, sizeof(size_t));
	data += sizeof(size_t);
	char* const psData = data;

	Material* const material {new Material{}};
	material->vertexLayout = vertexLayout;
	material->vertexShader = LowLevelRenderer::
								createVertexShader(vsData, vsSize);
	material->pixelShader = LowLevelRenderer::
								createPixelShader(psData, psSize);
	materials.push_back(material);
	return material;
}

void Clair::Renderer::setCameraMatrix(const Clair::Matrix& m) {
	LowLevelRenderer::setCameraMatrix(m);
	return;
}
