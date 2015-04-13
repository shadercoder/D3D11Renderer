#include "BasicSample.h"
#include "Clair/Renderer.h"
#include "Clair/Object.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/LoadBinaryData.h"
#include "Clair/Material.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

class CB_default {
public:
	Clair::Float4 DiffuseColor;
};

bool BasicSample::initialize(const HWND hwnd) {
	if (!Clair::Renderer::initialize(hwnd)) return false;

	auto bunnyMeshData = loadBinaryData("../../common/data/models/bunny.cmod");
	auto bunnyMesh = Clair::Renderer::createMesh(bunnyMeshData.data());

	auto defaultMatData =
		loadBinaryData("../../common/data/materials/default.cmat");
	auto defaultMat = Clair::Renderer::createMaterial(defaultMatData.data());
	defaultMat->setConstantBuffer<CB_default>();

	mScene = Clair::Renderer::createScene();
	for (int x {0}; x < 5; ++x) {
		for (int y {0}; y < 5; ++y) {
			const float fx = static_cast<float>(x);
			const float fy = static_cast<float>(y);
			const vec3 pos {fx * 3.0f, 0.0f, fy * 3.0f};
			auto obj = mScene->createObject();
			obj->setMesh(bunnyMesh);
			obj->setMatrix(value_ptr(translate(pos)));
			auto matInstance = obj->setMaterial(CLAIR_RENDER_PASS(0),
												defaultMat);
			CB_default* const cb = matInstance->setConstantBuffer<CB_default>();
			const float col[] {fx / 5.0f, fy / 5.0f, 0.1f, 1.0f};
			cb->DiffuseColor = col;
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

void BasicSample::update(const float deltaTime, const float ) {
	Camera::update(deltaTime);
}

void BasicSample::render() {
	Clair::Renderer::clear();
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::render(mScene);
	Clair::Renderer::finalizeFrame();
}
