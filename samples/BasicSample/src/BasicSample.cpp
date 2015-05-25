#include "BasicSample.h"
#include "Clair/Object.h"
#include <Clair/Clair.h>
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "Clair/Material.h"
#include "Clair/Mesh.h"
#include "../../data/materials/basic.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

bool BasicSample::initialize(const HWND hwnd) {
	if (!Clair::initialize(hwnd, Logger::log)) {
		return false;
	}

	auto bunnyMeshData = Loader::loadBinaryData("models/bunny.cmod");
	Clair::Mesh* bunnyMesh {Clair::ResourceManager::createMesh()};
	bunnyMesh->initialize(bunnyMeshData.get());

	auto materialData = Loader::loadBinaryData("materials/basic.cmat");
	Clair::Material* material {Clair::ResourceManager::createMaterial()};
	material->initialize(materialData.get());

	mScene = Clair::ResourceManager::createScene();
	mBunny = mScene->createObject();
	mBunny->setMesh(bunnyMesh);
	mBunny->setMatrix(glm::value_ptr(glm::translate(glm::vec3{0.0f})));

	Clair::MaterialInstance* matInstance {
		mBunny->setMaterial(CLAIR_RENDER_PASS(0), material)};
	mConstBuffer = matInstance->getConstantBufferPs<Cb_materials_basic_Ps>();
	mConstBuffer->DiffuseColor = {0.8f, 0.2f, 0.1f, 1.0f};

	Camera::initialize({-0.27f, 1.64f, -1.79f}, 0.470f, 0.045f);
	return true;
}

void BasicSample::terminate() {
	Clair::terminate();
}

void BasicSample::onResize() {
	Clair::Renderer::resizeSwapBuffer(getWidth(), getHeight());
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(90.0f), getAspect(), 0.01f, 100.0f)));
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
