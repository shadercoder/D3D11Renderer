#include "BasicSample.h"
#include "Clair/Renderer.h"
#include "Clair/Object.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "Clair/Material.h"
#include "../../data/materials/default.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

bool MaterialSample::initialize(const HWND hwnd) {
	if (!Clair::Renderer::initialize(hwnd, Logger::logCallback)) {
		return false;
	}

	auto bunnyMeshData = Loader::loadBinaryData("models/bunny.cmod");
	auto bunnyMesh = Clair::Renderer::createMesh(bunnyMeshData.data());

	auto defaultMatData = Loader::loadBinaryData("materials/default.cmat");
	auto defaultMat = Clair::Renderer::createMaterial(defaultMatData.data());

	mScene = Clair::Renderer::createScene();
	mBunny = mScene->createObject();
	mBunny->setMesh(bunnyMesh);
	mBunny->setMatrix(value_ptr(translate(vec3{0.0f})));
	auto matInstance = mBunny->setMaterial(CLAIR_RENDER_PASS(0), defaultMat);
	mConstBuffer = matInstance->getConstantBufferPs<Cb_materials_default_Ps>();
	mConstBuffer->DiffuseColor = Clair::Float4{0.8f, 0.2f, 0.1f, 1.0f};

	Camera::initialize({-0.27f, 1.64f, -1.79f}, 0.470f, 0.045f);
	return true;
}

void MaterialSample::terminate() {
	Clair::Renderer::terminate();
}

void MaterialSample::onResize(const int width, const int height,
						   const float aspect) {
	Clair::Renderer::setViewport(0, 0, width, height);
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(90.0f), aspect, 0.01f, 100.0f)));
}

void MaterialSample::update() {
	mConstBuffer->Time = getRunningTime();
	Camera::update(getDeltaTime());
}

void MaterialSample::render() {
	Clair::Renderer::clear(true);
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::render(mScene);
	Clair::Renderer::finalizeFrame();
}
