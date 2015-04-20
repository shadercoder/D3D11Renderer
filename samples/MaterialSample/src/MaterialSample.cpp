#include "MaterialSample.h"
#include "Clair/Renderer.h"
#include "Clair/Object.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "Clair/Material.h"
#include "../../data/materials/pbrSimple.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

bool MaterialSample::initialize(const HWND hwnd) {
	if (!Clair::Renderer::initialize(hwnd, Logger::logCallback)) {
		return false;
	}

	auto test = Loader::loadImageData("textures/avatar.png");
	auto texture = Clair::Renderer::createTexture(test.get());

	auto sphereMeshData = Loader::loadBinaryData("models/sphere.cmod");
	auto sphereMesh = Clair::Renderer::createMesh(sphereMeshData.get());

	auto matData = Loader::loadBinaryData("materials/pbrSimple.cmat");
	auto material = Clair::Renderer::createMaterial(matData.get());

	auto skyMatData = Loader::loadBinaryData("materials/sky.cmat");
	auto skyMaterial = Clair::Renderer::createMaterial(skyMatData.get());

	mScene = Clair::Renderer::createScene();
	const int size = 5;
	const float fsize = static_cast<float>(size);
	for (int x = 0; x < size; ++x) {
	for (int y = 0; y < size; ++y) {
	for (int z = 0; z < size; ++z) {
		const float fx = static_cast<float>(x) / (fsize - 1);
		const float fy = static_cast<float>(y) / (fsize - 1);
		const float fz = static_cast<float>(z) / (fsize - 1);
		Clair::Object* const obj = mScene->createObject();
		obj->setMesh(sphereMesh);
		obj->setMatrix(value_ptr(translate(vec3{fx, fy, fz} * fsize * 2.2f)));
		auto matInst = obj->setMaterial(CLAIR_RENDER_PASS(0), material);
		matInst->setTexture(0, texture);
		auto cbuf = matInst->getConstantBufferPs<Cb_materials_pbrSimple_Ps>();
		cbuf->Reflectivity = fx;
		cbuf->Roughness = fy;
		cbuf->Metalness = fz;
	}}}

	mSkyMaterialInstance = Clair::Renderer::createMaterialInstance(skyMaterial);
	mSkyMaterialInstance->setTexture(0, texture);
	mSkyConstBuffer =
		mSkyMaterialInstance->getConstantBufferPs<Cb_materials_sky_Ps>();

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
	mSkyConstBuffer->Aspect = aspect;
}

void MaterialSample::update() {
	Camera::update(getDeltaTime());
	mSkyConstBuffer->CamRight = value_ptr(Camera::getRight());
	mSkyConstBuffer->CamUp = value_ptr(Camera::getUp());
	mSkyConstBuffer->CamForward = value_ptr(Camera::getForward());
}

void MaterialSample::render() {
	Clair::Renderer::clear(true);
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::setCameraPosition(value_ptr(Camera::getPosition()));
	Clair::Renderer::renderScreenQuad(mSkyMaterialInstance);
	Clair::Renderer::clear(false);
	Clair::Renderer::render(mScene);
	Clair::Renderer::finalizeFrame();
}
