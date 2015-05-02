#include "BasicSample.h"
#include "Clair/Object.h"
#include <Clair/Clair.h>
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "Clair/Material.h"
#include "Clair/Mesh.h"
#include "../../data/materials/default.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

bool BasicSample::initialize(const HWND hwnd) {
	if (!Clair::initialize(hwnd, Logger::log)) {
		return false;
	}

	auto bunnyMeshData = Loader::loadBinaryData("models/bunny.cmod");
	auto bunnyMesh = Clair::ResourceManager::createMesh();
	bunnyMesh->initialize(bunnyMeshData.get());

	auto defaultMatData = Loader::loadBinaryData("materials/default.cmat");
	auto defaultMat = Clair::ResourceManager::createMaterial();
	defaultMat->initialize(defaultMatData.get());

	mScene = Clair::ResourceManager::createScene();
	mBunny = mScene->createObject();
	mBunny->setMesh(bunnyMesh);
	mBunny->setMatrix(value_ptr(translate(vec3{0.0f})));
	auto matInstance = mBunny->setMaterial(CLAIR_RENDER_PASS(0), defaultMat);
	mConstBuffer = matInstance->getConstantBufferPs<Cb_materials_default_Ps>();
	mConstBuffer->DiffuseColor = Clair::Float4{0.8f, 0.2f, 0.1f, 1.0f};

	Camera::initialize({-0.27f, 1.64f, -1.79f}, 0.470f, 0.045f);
	return true;
}

void BasicSample::terminate() {
	Clair::terminate();
}

void BasicSample::onResize(const int width, const int height,
						   const float aspect) {
	Clair::Renderer::setViewport(0, 0, width, height);
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(90.0f), aspect, 0.01f, 100.0f)));
}

void BasicSample::update() {
	mConstBuffer->Time = getRunningTime();
	Camera::update(getDeltaTime());
}

void BasicSample::render() {
	Clair::Renderer::clearColor({0.2f, 0.4f, 0.6f, 1.0f});
	Clair::Renderer::clearDepthStencil(1.0f, 0);
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::render(mScene);
	Clair::Renderer::finalizeFrame();
}
