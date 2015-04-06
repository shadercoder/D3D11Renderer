#include "Sample.h"
#include "Clair/Renderer.h"
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include "GlmMath.h"
#include "Camera.h"
#include "LoadBinaryData.h"
#include "Input.h"

using namespace glm;

Clair::Scene* scene {nullptr};

bool Sample::initialize(HWND hwnd) {
	// Initialize Clair using the window's HWND.
	if (!Clair::Renderer::initialize(hwnd)) return false;

	// Load mesh from file and send it to Clair as byte array (char*).
	auto meshData = loadBinaryData("../data/model.cmod");
	auto mesh = Clair::Renderer::createMesh(meshData.data());

	// Load materials from file and send them to Clair as byte array (char*).
	auto defaultMatData = loadBinaryData("../data/default.cmat");
	auto defaultMat = Clair::Renderer::createMaterial(defaultMatData.data());
	auto normalsMatData = loadBinaryData("../data/normals.cmat");
	auto normalsMat = Clair::Renderer::createMaterial(normalsMatData.data());

	// Create a scene and a object with the mesh and materials assigned.
	scene = Clair::Renderer::createScene();
	Clair::Object* obj {scene->createObject()};
	obj->setMesh(mesh);
	// Different materials can be assigned to different render passes.
	obj->setMaterial(Clair::RenderPass::DEFAULT, defaultMat);
	obj->setMaterial(Clair::RenderPass::NORMALS_ONLY, normalsMat);
	obj->setMatrix(Clair::Matrix(value_ptr(mat4{})));

	return true;
}

void Sample::terminate() {
	Clair::Renderer::terminate();
}

void Sample::update(const float deltaTime, const float ) {
	Camera::update(deltaTime);
	if (Input::getKeyDown(SDL_SCANCODE_N)) {
		Clair::Renderer::setRenderPass(Clair::RenderPass::DEFAULT);
	} else if (Input::getKeyDown(SDL_SCANCODE_M)) {
		Clair::Renderer::setRenderPass(Clair::RenderPass::NORMALS_ONLY);
	}
}

void Sample::render() {
	Clair::Renderer::clear();
	Clair::Renderer::setCameraMatrix(
							Clair::Matrix(value_ptr(Camera::getViewMatrix())));
	Clair::Renderer::render(scene);
	Clair::Renderer::finalizeFrame();
}

void Sample::onResize(const int width, const int height) {
	Clair::Renderer::setViewport(0, 0, width, height);
}