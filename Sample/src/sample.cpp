#include <Windows.h>
#include "sample.h"
#include "clair/renderer.h"
#include "clair/scene.h"
#include "clair/object.h"
#include "clair/vertexBuffer.h"
#include <fstream>
#include <sstream>
#include "glmMath.h"
#include "SDL2/SDL.h"
#include "input.h"
#include "camera.h"

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

Clair::Scene* scene = nullptr;

bool Sample::initialize(HWND hwnd) {
	if (!Clair::Renderer::initialize(hwnd)) return false;

	scene = Clair::Renderer::createScene();

	Clair::VertexShader* vs = nullptr;
	Clair::PixelShader* ps = nullptr;
	auto byteCode = readBytes("../data/material.csm");
	Clair::Renderer::createMaterial(byteCode.data(), vs, ps);
	auto mesh = loadMesh("../data/bunny.cmd", vs);

	const int size = 2;
	for (int x = -size; x < size; ++x) {
		const float fx = static_cast<float>(x) * 3.0f;
		for (int y = -size; y < size; ++y) {
			Clair::Object* cube = scene->createObject();
			const float fz = static_cast<float>(y) * 3.0f;
			const mat4 m = translate(vec3(fx, 0.0f, fz));
			cube->setMatrix(Clair::Matrix(value_ptr(m)));
			cube->setMesh(mesh);
		}
	}
	return true;
}

void Sample::terminate() {
	Clair::Renderer::terminate();
}

void Sample::update(const float deltaTime, const float ) {
	Camera::update(deltaTime);
}

void Sample::render() {
	Clair::Renderer::clear();
	Clair::Renderer::setCameraMatrix(Clair::Matrix(
										value_ptr(Camera::getViewMatrix())));
	Clair::Renderer::render(scene);
	Clair::Renderer::finalizeFrame();
}

void Sample::onResize(const float width, const float height) {
	Clair::Renderer::setViewport(0, 0, width, height);
}