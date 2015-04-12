#include "BasicSample.h"
#include "Clair/Renderer.h"
#include "Clair/Object.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/LoadBinaryData.h"
#include "SampleFramework/Input.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

CLAIR_RENDER_PASS_ENUM (Pass) {
	DEFAULT,
	NORMALS
};

bool BasicSample::initialize(const HWND hwnd) {
	// Initialize Clair using the window's HWND.
	if (!Clair::Renderer::initialize(hwnd)) return false;

	// Load meshes from file and send it to Clair as byte array (char*).
	auto planeMeshData = loadBinaryData("../../common/data/models/plane.cmod");
	auto planeMesh = Clair::Renderer::createMesh(planeMeshData.data());
	auto bunnyMeshData = loadBinaryData("../../common/data/models/bunny.cmod");
	auto bunnyMesh = Clair::Renderer::createMesh(bunnyMeshData.data());

	// Load materials from file and send them to Clair as byte array (char*).
	auto defaultMatData =
		//loadBinaryData("../../common/data/materials/default.cmat");
		loadBinaryData("../../common/data/materials/default.cmat");
	auto defaultMat = Clair::Renderer::createMaterial(defaultMatData.data());
	auto normalsMatData =
		loadBinaryData("../../common/data/materials/normals.cmat");
	auto normalsMat = Clair::Renderer::createMaterial(normalsMatData.data());

	// Create a scene and objects with the meshes and materials assigned.
	mScene = Clair::Renderer::createScene();
	Clair::Object* const plane {mScene->createObject()};
	plane->setMesh(planeMesh);
	plane->setMatrix(value_ptr(scale(vec3{1.0f} * 1.0f)));
	plane->setMaterial(CLAIR_RENDER_PASS(Pass::DEFAULT), defaultMat);
	plane->setMaterial(CLAIR_RENDER_PASS(Pass::NORMALS), normalsMat);
	Clair::Object* const bunny {mScene->createObject()};
	bunny->setMesh(bunnyMesh);
	bunny->setMatrix(value_ptr(translate(vec3{0.0f, 0.0f, 0.0f})));
	bunny->setMaterial(CLAIR_RENDER_PASS(Pass::DEFAULT), defaultMat);
	bunny->setMaterial(CLAIR_RENDER_PASS(Pass::NORMALS), normalsMat);

	Camera::initialize(vec3{-0.31f, 1.68f, -2.21f}, 0.415f, 0.015f);
	return true;
}

void BasicSample::terminate() {
	Clair::Renderer::terminate();
}

void BasicSample::onResize(const int width, const int height,
						   const float aspect) {
	Clair::Renderer::setViewport(0, 0, width, height);
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(90.0f), aspect, 0.01f, 100.0f)));
}

void BasicSample::update(const float deltaTime, const float ) {
	// Update camera matrix and current render pass based on input.
	Camera::update(deltaTime);
	if (Input::getKeyDown(SDL_SCANCODE_1)) {
		Clair::Renderer::setRenderPass(CLAIR_RENDER_PASS(Pass::DEFAULT));
	} else if (Input::getKeyDown(SDL_SCANCODE_2)) {
		Clair::Renderer::setRenderPass(CLAIR_RENDER_PASS(Pass::NORMALS));
	}
}

void BasicSample::render() {
	// Render the scene(s)
	Clair::Renderer::clear();
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::render(mScene);
	Clair::Renderer::finalizeFrame();
}
