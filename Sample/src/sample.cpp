#include <Windows.h>
#include "sample.h"
#include "clair/renderer.h"
#include "clair/scene.h"
#include "clair/object.h"
#include "clair/vertexBuffer.h"
#include <fstream>
#include <sstream>

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

Clair::Mesh* mesh = nullptr;//loadMesh("../data/test.txt", vs);

void loadMaterial(const std::string& filename, Clair::VertexShader*& vs,
				  Clair::PixelShader*& ps) {
	auto byteCode = readBytes(filename);
	Clair::Renderer::createMaterial(byteCode.data(), vs, ps);
	mesh = loadMesh("../data/test.txt", vs);
}

Clair::Scene* scene0 = nullptr;
Clair::Scene* scene1 = nullptr;

void Sample::initialize() {
	float m[16] = {	1.0f, 0.0f,  0.0f,  0.0f,
					0.0f, 1.0f,  0.0f,  0.0f,
					0.0f, 0.0f,  1.0f,  0.0f,
					0.0f, -0.4f, 0.0f,  1.0f };
	float mplane[16] = { 50.0f, 0.0f, 0.0f,  0.0f,
						 0.0f,  0.1f, 0.0f,  0.0f,
						 0.0f,  0.0f, 50.0f, 0.0f,
						 0.0f, -1.0f, 0.0f,  1.0f };

	scene0 = Clair::Renderer::createScene();
	scene1 = Clair::Renderer::createScene();

	// shaders
	Clair::VertexShader* vs = nullptr;
	Clair::PixelShader* ps = nullptr;
	loadMaterial("../data/test.csm", vs, ps);

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
}

void Sample::terminate() {
}

void Sample::update() {
}

void Sample::render() {
	Clair::Renderer::clear();
	Clair::Renderer::setCameraMatrix(Clair::Matrix());
	//Clair::Renderer::render(scene0);
	Clair::Renderer::render(scene1);
}

void Sample::onResize(const float width, const float height) {
	Clair::Renderer::setViewport(0, 0, width, height);
}