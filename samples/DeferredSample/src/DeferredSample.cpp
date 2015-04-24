#include "DeferredSample.h"
#include "Clair/Object.h"
#include <Clair/Clair.h>
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "Clair/Material.h"
#include "Clair/Mesh.h"
//#include "../../data/materials/pbrSimple.h"
#include "../../data/materials/deferredGeometry.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

void DeferredSample::createRenderTarget(Clair::RenderTarget*& outRenderTarget,
										Clair::Texture*& outTexture) const {
	outTexture = Clair::ResourceManager::createTexture();
	outTexture->initialize(960, 640, nullptr,
		Clair::Texture::Type::RENDER_TARGET);
	outRenderTarget = Clair::ResourceManager::createRenderTarget();
	outRenderTarget->initialize(outTexture);
}

bool DeferredSample::initialize(const HWND hwnd) {
	if (!Clair::initialize(hwnd, Logger::log)) {
		return false;
	}
	// Create render targets
	createRenderTarget(mGBufAlbedo, mGBufAlbedoTex);
	createRenderTarget(mGBufNormal, mGBufNormalTex);
	createRenderTarget(mGBufPosition, mGBufPositionTex);
	auto depthTex = Clair::ResourceManager::createTexture();
	depthTex->initialize(960, 640, nullptr,
		Clair::Texture::Type::DEPTH_STENCIL_TARGET);
	mGBufDepthStencil = Clair::ResourceManager::createDepthStencilTarget();
	mGBufDepthStencil->initialize(depthTex);

	// Group them into a GBuffer
	mGBuffer = new Clair::RenderTargetGroup{3};
	mGBuffer->setRenderTarget(0, mGBufAlbedo);
	mGBuffer->setRenderTarget(1, mGBufNormal);
	mGBuffer->setRenderTarget(2, mGBufPosition);
	mGBuffer->setDepthStencilTarget(mGBufDepthStencil);

	auto compTexData = Loader::loadBinaryData("materials/deferredComposite.cmat");
	auto compTex = Clair::ResourceManager::createMaterial();
	compTex->initialize(compTexData.get());
	mDeferredCompositeMat = Clair::ResourceManager::createMaterialInstance();
	mDeferredCompositeMat->initialize(compTex);
	mDeferredCompositeMat->setTexture(0, mGBufAlbedoTex);
	mDeferredCompositeMat->setTexture(1, mGBufNormalTex);
	mDeferredCompositeMat->setTexture(2, mGBufPositionTex);

	auto bunnyMeshData = Loader::loadBinaryData("models/sphere.cmod");
	auto bunnyMesh = Clair::ResourceManager::createMesh();
	bunnyMesh->initialize(bunnyMeshData.get());
	auto planeMeshData = Loader::loadBinaryData("models/plane.cmod");
	auto planeMesh = Clair::ResourceManager::createMesh();
	planeMesh->initialize(planeMeshData.get());

	auto geometryMatData =
		Loader::loadBinaryData("materials/deferredGeometry.cmat");
	auto geometryMat = Clair::ResourceManager::createMaterial();
	geometryMat->initialize(geometryMatData.get());

	mScene = Clair::ResourceManager::createScene();
	auto bunny = mScene->createObject();
	bunny->setMesh(bunnyMesh);
	bunny->setMatrix(value_ptr(translate(vec3{0.0f})));
	auto matInstance = bunny->setMaterial(CLAIR_RENDER_PASS(0), geometryMat);
	mConstBuffer =
		matInstance->getConstantBufferPs<Cb_materials_deferredGeometry_Ps>();
	mConstBuffer->DiffuseColor = Clair::Float4{0.8f, 0.2f, 0.1f, 1.0f};
	auto plane = mScene->createObject();
	plane->setMesh(planeMesh);
	plane->setMatrix(value_ptr(translate(vec3{0.0f})));
	auto planeMatInstance = plane->setMaterial(CLAIR_RENDER_PASS(0), geometryMat);
	mConstBuffer =
		planeMatInstance->getConstantBufferPs<Cb_materials_deferredGeometry_Ps>();
	mConstBuffer->DiffuseColor = Clair::Float4{0.8f, 0.5f, 0.5f, 1.0f};

	Camera::initialize({-0.27f, 1.64f, -1.79f}, 0.470f, 0.045f);
	return true;
}

void DeferredSample::terminate() {
	delete mGBuffer;
	Clair::terminate();
}

void DeferredSample::onResize(const int width, const int height,
						   const float aspect) {
	Clair::Renderer::setViewport(0, 0, width, height);
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(90.0f), aspect, 0.01f, 100.0f)));
}

void DeferredSample::update() {
	Camera::update(getDeltaTime());
}

void DeferredSample::render() {
	// Render to G-buffer
	Clair::Renderer::setRenderTargetGroup(mGBuffer);
	mGBufAlbedo->clear({0.0f, 0.0f, 0.0f, 1.0f});
	mGBufNormal->clear({0.0f, 0.0f, 0.0f, 1.0f});
	mGBufPosition->clear({0.0f, 0.0f, 0.0f, 1.0f});
	mGBufDepthStencil->clearDepth(1.0f);
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::setCameraPosition(value_ptr(Camera::getPosition()));
	Clair::Renderer::render(mScene);

	// Composite
	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::clear(true);
	Clair::Renderer::renderScreenQuad(mDeferredCompositeMat);

	Clair::Renderer::finalizeFrame();
}
