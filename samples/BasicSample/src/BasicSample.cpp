#include "BasicSample.h"
#include "Clair/Renderer.h"
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include "SampleCommon/GlmMath.h"
#include "SampleCommon/Camera.h"
#include "SampleCommon/LoadBinaryData.h"
#include "SampleCommon/Input.h"
//#include "vld.h"

using namespace SampleCommon;
using namespace glm;

CLAIR_RENDER_PASS_ENUM (Pass) {
	DEFAULT,
	NORMALS
};

Clair::Scene* scene {nullptr};

bool BasicSample::initialize(const HWND hwnd) {
	// Initialize Clair using the window's HWND.
	if (!Clair::Renderer::initialize(hwnd)) return false;

	// Load meshes from file and send it to Clair as byte array (char*).
	auto planeMeshData = loadBinaryData("../data/models/plane.cmod");
	auto planeMesh = Clair::Renderer::createMesh(planeMeshData.data());
	auto bunnyMeshData = loadBinaryData("../data/models/bunny.cmod");
	auto bunnyMesh = Clair::Renderer::createMesh(bunnyMeshData.data());

	// Load materials from file and send them to Clair as byte array (char*).
	auto defaultMatData = loadBinaryData("../data/materials/default.cmat");
	auto defaultMat = Clair::Renderer::createMaterial(defaultMatData.data());
	auto normalsMatData = loadBinaryData("../data/materials/normals.cmat");
	auto normalsMat = Clair::Renderer::createMaterial(normalsMatData.data());

	// Create a scene and objects with the meshes and materials assigned.
	scene = Clair::Renderer::createScene();
	Clair::Object* plane {scene->createObject()};
	plane->setMesh(planeMesh);
	plane->setMatrix(Clair::Matrix{value_ptr(scale(vec3{1.0f} * 10.0f))});
	plane->setMaterial(CLAIR_RENDER_PASS(Pass::DEFAULT), defaultMat);
	plane->setMaterial(CLAIR_RENDER_PASS(Pass::NORMALS), normalsMat);
	Clair::Object* bunny {scene->createObject()};
	bunny->setMesh(bunnyMesh);
	bunny->setMatrix(Clair::Matrix(value_ptr(translate(vec3{5.0f, 0.0f, 5.0f}))));
	bunny->setMaterial(CLAIR_RENDER_PASS(Pass::DEFAULT), defaultMat);
	bunny->setMaterial(CLAIR_RENDER_PASS(Pass::NORMALS), normalsMat);

	return true;
}

void BasicSample::terminate() {
	Clair::Renderer::terminate();
}

void BasicSample::update(const float deltaTime, const float ) {
	// Update camera matrix and current render pass based on input.
	Camera::update(deltaTime);
	if (Input::getKeyDown(SDL_SCANCODE_N)) {
		Clair::Renderer::setRenderPass(CLAIR_RENDER_PASS(Pass::DEFAULT));
	} else if (Input::getKeyDown(SDL_SCANCODE_M)) {
		Clair::Renderer::setRenderPass(CLAIR_RENDER_PASS(Pass::NORMALS));
	}
}

void BasicSample::render() {
	// Render the scene(s)
	Clair::Renderer::clear();
	Clair::Renderer::setCameraMatrix(
							Clair::Matrix(value_ptr(Camera::getViewMatrix())));
	Clair::Renderer::render(scene);
	Clair::Renderer::finalizeFrame();
}

void BasicSample::onResize(const int width, const int height) {
	Clair::Renderer::setViewport(0, 0, width, height);
}