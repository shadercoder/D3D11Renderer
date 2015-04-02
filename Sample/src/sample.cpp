#include <Windows.h>
#include "sample.h"
#include "clair/renderer.h"
#include "clair/scene.h"
#include "clair/object.h"
#include "clair/vertexBuffer.h"
#include <fstream>
#include <sstream>
#pragma warning(push, 3)
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#pragma warning(pop)

using namespace glm;

static std::vector<char> readBytes(const std::string& filename) {
	std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		printf("Couldn't open file\n");
		return std::vector<char>();
	}
	const auto pos = file.tellg();
	std::vector<char> vec(static_cast<unsigned>(pos));
	file.seekg(0, std::ios::beg);
	file.read(&vec[0], pos);
	return vec;
}

static Clair::Mesh* loadMesh(const std::string& filename,
							 Clair::VertexShader* vs) {
	if (!vs) return nullptr;
	FILE* file;
	fopen_s(&file, filename.c_str(), "rb");
	if (!file) return nullptr;
	unsigned numVertices = 0;
	fread(&numVertices, sizeof(unsigned), 1, file);
	void* vertData = new float[numVertices * 3 * 2];
	fread(vertData, sizeof(float), numVertices * 3 * 2, file);
	unsigned numIndices = 0;
	fread(&numIndices, sizeof(unsigned), 1, file);
	unsigned* indData = new unsigned[numIndices];
	fread(indData, sizeof(unsigned), numIndices, file);
	fclose(file);

	Clair::VertexLayout vertexLayout;
	vertexLayout.addElement({"POSITION",
							 Clair::VertexLayout::Element::Format::FLOAT3, 0});
	vertexLayout.addElement({"NORMAL",
							 Clair::VertexLayout::Element::Format::FLOAT3, 12});
	//vertexLayout.addElement({"BLA",
	//						 Clair::VertexLayout::Element::Format::FLOAT2, 0});
	auto const inputLayout = Clair::Renderer::createInputLayout(vertexLayout,
																vs);

	Clair::MeshDesc meshDesc;
	meshDesc.inputLayout = inputLayout;
	meshDesc.vertexData = vertData;
	meshDesc.vertexDataSize = sizeof(float) * numVertices * 3 * 2;
	meshDesc.indexData = indData;
	meshDesc.numIndices = numIndices;

	Clair::Mesh* const newMesh = Clair::Renderer::createMesh(meshDesc);
	delete[] meshDesc.vertexData;
	delete[] meshDesc.indexData;
	return newMesh;
}

Clair::Scene* scene0 = nullptr;
Clair::Scene* scene1 = nullptr;

bool Sample::initialize(HWND hwnd) {
	if (!Clair::Renderer::initialize(hwnd)) return false;

	float m[16] = { 0.0f };
	float mplane[16] = { 0.0f };
	memcpy(&m, value_ptr(translate(vec3(0.0f, -0.4f, 0.0f))),
		   sizeof(float) * 16);
	memcpy(&mplane, value_ptr(translate(vec3(0.0f, -0.4f, 0.0f)) *
		   scale(vec3(50.0f, 0.1f, 50.0f))),
		   sizeof(float) * 16);

	scene0 = Clair::Renderer::createScene();
	scene1 = Clair::Renderer::createScene();

	// shaders
	Clair::VertexShader* vs = nullptr;
	Clair::PixelShader* ps = nullptr;
	auto byteCode = readBytes("../data/material.csm");
	Clair::Renderer::createMaterial(byteCode.data(), vs, ps);
	auto mesh = loadMesh("../data/bunny.cmd", vs);

	// ground plane
	Clair::Object* plane = scene0->createObject();
	plane->setMatrix(Clair::Matrix(mplane));
	plane->setMesh(mesh);

	// cubes
	const int size = 1;
	for (int x = -size; x < size; ++x) {
		m[12] = static_cast<float>(x) * 3.0f;
		for (int y = -size; y < size; ++y) {
			Clair::Object* cube = scene1->createObject();
			m[14] = static_cast<float>(y) * 3.0f;
			cube->setMatrix(Clair::Matrix(m));
			cube->setMesh(mesh);
		}
	}
	return true;
}

void Sample::update() {
}

void Sample::render() {
	Clair::Renderer::clear();
	Clair::Renderer::setCameraMatrix(Clair::Matrix());
	Clair::Renderer::render(scene0);
	Clair::Renderer::render(scene1);
	Clair::Renderer::finalizeFrame();
}

void Sample::terminate() {
	Clair::Renderer::terminate();
}

void Sample::onResize(const float width, const float height) {
	Clair::Renderer::setViewport(0, 0, width, height);
}