#include <Windows.h>
#include "sample.h"
#include "clair/renderer.h"
#include "clair/scene.h"
#include "clair/object.h"
#include "clair/camera.h"
#include "clair/vertexBuffer.h"
#include <fstream>
//#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct Vertex {
	XMFLOAT3 position;
	XMFLOAT2 uvs;
};

Vertex vertices[] = {
	{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
	{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
	{ XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
	{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },

	{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
	{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
	{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
	{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

	{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
	{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
	{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
	{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },

	{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
	{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
	{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
	{ XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },

	{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
	{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
	{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
	{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },

	{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
	{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
	{ XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
	{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
};

UINT indices[] = {
	3,1,0, 2,1,3,
	6,4,5, 7,4,6,
	11,9,8, 10,9,11,
	14,12,13, 15,12,14,
	19,17,16, 18,17,19, 
	22,20,21, 23,20,22
};

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

Clair::Scene* scene0 = nullptr;
Clair::Scene* scene1 = nullptr;

void Sample::initialize() {
	float m[16] = {	0.4f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.4f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.4f, 0.0f,
					0.0f, -0.4f, 0.0f, 1.0f };
	float mplane[16] = {	50.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.1f, 0.0f, 0.0f,
							0.0f, 0.0f, 50.0f, 0.0f,
							0.0f, -1.0f, 0.0f, 1.0f };

	scene0 = Clair::Renderer::createScene();
	scene1 = Clair::Renderer::createScene();

	// shaders
	auto vsCode = readBytes("../data/shaders/VertexShader.cso");
	auto psCode = readBytes("../data/shaders/PixelShader.cso");
	Clair::VertexShader* vs = Clair::Renderer::createVertexShader(vsCode);
	Clair::Renderer::createPixelShader(psCode);

	Clair::InputLayoutDesc inputLayoutDesc;
	inputLayoutDesc.addElement({"POSITION", Clair::InputLayoutDesc::Element::Format::FLOAT3, 0});
	inputLayoutDesc.addElement({"TEXCOORD", Clair::InputLayoutDesc::Element::Format::FLOAT2, 12});
	Clair::InputLayout* const inputLayout = Clair::Renderer::createInputLayout(inputLayoutDesc, vs);

	// meshes
	Clair::MeshDesc meshDesc;
	meshDesc.inputLayout = inputLayout;
	meshDesc.vertexData = vertices;
	meshDesc.vertexDataSize = sizeof(Vertex) * 24;
	meshDesc.indexData = indices;
	meshDesc.numIndices = 36;
	Clair::Mesh* mesh = Clair::Renderer::createMesh(meshDesc);

	// ground plane
	Clair::Object* plane = scene0->createObject();
	plane->setMatrix(Clair::Matrix(mplane));
	plane->setMesh(mesh);

	// cubes
	const int size = 3;
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
	Clair::Renderer::render(scene0);
	Clair::Renderer::render(scene1);
}

void Sample::onResize(const float width, const float height) {
	Clair::Renderer::setViewport(0, 0, width, height);
}