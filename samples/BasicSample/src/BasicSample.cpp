#include "BasicSample.h"
#include "Clair/Renderer.h"
#include "Clair/Object.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/LoadBinaryData.h"
#include "SampleFramework/Logger.h"
#include "Clair/Material.h"
#include "../../data/materials/default.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

bool BasicSample::initialize(const HWND hwnd) {
	if (!Clair::Renderer::initialize(hwnd, Logger::logCallback)) {
		return false;
	}

	auto bunnyMeshData = loadBinaryData("../../common/data/models/bunny.cmod");
	auto bunnyMesh = Clair::Renderer::createMesh(bunnyMeshData.data());

	auto defaultMatData =
		loadBinaryData("../../common/data/materials/default.cmat");
	auto defaultMat = Clair::Renderer::createMaterial(defaultMatData.data());

	mScene = Clair::Renderer::createScene();
	for (int x {0}; x < 5; ++x) {
		for (int y {0}; y < 5; ++y) {
			const float fx = static_cast<float>(x);
			const float fy = static_cast<float>(y);
			const vec3 pos {fx * 2.5f, 0.0f, fy * 2.0f};
			auto obj = mScene->createObject();
			obj->setMesh(bunnyMesh);
			obj->setMatrix(value_ptr(translate(pos)));
			auto matInstance = obj->setMaterial(CLAIR_RENDER_PASS(0),
												defaultMat);
			auto const cb = matInstance->
								getConstantBufferPs<Cb_materials_default_Ps>();
			cb->DiffuseColor = Clair::Float4{fx / 5.0f, fy / 5.0f, 0.1f, 1.0f};
		}
	}
	Camera::initialize({2.2f, 4.3f, -2.28f}, 0.625f, 0.320f);
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

void BasicSample::update() {
	Camera::update(getDeltaTime());
}

void BasicSample::render() {
	Clair::Renderer::clear();
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::render(mScene);
	Clair::Renderer::finalizeFrame();
}
