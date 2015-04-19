#include "MaterialSample.h"
#include "Clair/Renderer.h"
#include "Clair/Object.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "Clair/Material.h"
#include "../../data/materials/pbrSimple.h"
#include "vld.h"

using namespace SampleFramework;
using namespace glm;

bool MaterialSample::initialize(const HWND hwnd) {
	if (!Clair::Renderer::initialize(hwnd, Logger::logCallback)) {
		return false;
	}

	auto test = Loader::loadImageData("textures/grid.png");
	auto texture = Clair::Renderer::createTexture(test.get());

	auto sphereMeshData = Loader::loadBinaryData("models/sphere.cmod");
	auto sphereMesh = Clair::Renderer::createMesh(sphereMeshData.data());

	auto matData = Loader::loadBinaryData("materials/pbrSimple.cmat");
	auto material = Clair::Renderer::createMaterial(matData.data());

	auto skyMatData = Loader::loadBinaryData("materials/sky.cmat");
	auto skyMaterial = Clair::Renderer::createMaterial(skyMatData.data());

	mScene = Clair::Renderer::createScene();
	const int size = 5;
	const float fsize = static_cast<float>(size);
	for (int x = 0; x < size; ++x) {
	for (int y = 0; y < size; ++y) {
	for (int z = 0; z < size; ++z) {
		const float fx = static_cast<float>(x) / fsize;
		const float fy = static_cast<float>(y) / fsize;
		const float fz = static_cast<float>(z) / fsize;
		Clair::Object* const obj = mScene->createObject();
		obj->setMesh(sphereMesh);
		obj->setMatrix(value_ptr(translate(vec3{fx, fy, fz} * fsize * 3.0f)));
		auto matInst = obj->setMaterial(CLAIR_RENDER_PASS(0), material);
		matInst->setTexture(0, texture);
		auto cbuf = matInst->getConstantBufferPs<Cb_materials_pbrSimple_Ps>();
		cbuf->Reflectivity = fx;
		cbuf->Roughness = fy;
		cbuf->Metalness = fz;
	}}}

	mSkyMaterialInstance = Clair::Renderer::createMaterialInstance(skyMaterial);
	mSkyMaterialInstance->setTexture(0, texture);

	Camera::initialize({-4.5f, 16.6f, -4.5f}, 0.705f, 0.770f);
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
	Camera::update(getDeltaTime());
}

void MaterialSample::render() {
	Clair::Renderer::clear();
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::setCameraPosition(value_ptr(Camera::getPosition()));
	Clair::Renderer::render(mScene);
	//Clair::Renderer::renderScreenQuad(mSkyMaterialInstance);
	Clair::Renderer::finalizeFrame();
}
