#include "DeferredSample.h"
#include "Clair/Object.h"
#include <Clair/Clair.h>
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "Clair/Material.h"
#include "Clair/Mesh.h"
#include "Clair/RenderTargetGroup.h"
//#include "../../data/materials/pbrSimple.h"
#include "../../data/materials/default.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

Clair::RenderTarget* mRenderTarget {nullptr};
Clair::DepthStencilTarget* mDepthTarget {nullptr};
Clair::RenderTargetGroup* mRenderTargetGroup {nullptr};

bool DeferredSample::initialize(const HWND hwnd) {
	if (!Clair::initialize(hwnd, Logger::log)) {
		return false;
	}
	auto loadedTex = Loader::loadImageData("textures/avatar.png");
	auto texture = Clair::ResourceManager::createTexture();
	texture->initialize(
		loadedTex.width, loadedTex.height, loadedTex.data,
		Clair::Texture::Type::RENDER_TARGET);
	auto depthTex = Clair::ResourceManager::createTexture();
	depthTex->initialize(512, 512, nullptr,
		Clair::Texture::Type::DEPTH_STENCIL_TARGET);
	mRenderTarget = Clair::ResourceManager::createRenderTarget();
	mRenderTarget->initialize(texture);
	mDepthTarget = Clair::ResourceManager::createDepthStencilTarget();
	mDepthTarget->initialize(depthTex);
	mRenderTargetGroup = new Clair::RenderTargetGroup{1};
	mRenderTargetGroup->setRenderTarget(0, mRenderTarget);
	mRenderTargetGroup->setDepthStencilTarget(mDepthTarget);

	auto drawTexMatData = Loader::loadBinaryData("materials/drawTexture.cmat");
	auto drawTexMat = Clair::ResourceManager::createMaterial();
	drawTexMat->initialize(drawTexMatData.get());
	mDrawTexture = Clair::ResourceManager::createMaterialInstance();
	mDrawTexture->initialize(drawTexMat);
	mDrawTexture->setTexture(0, texture);

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

void DeferredSample::terminate() {
	delete mRenderTargetGroup;
	Clair::terminate();
}

void DeferredSample::onResize(const int width, const int height,
						   const float aspect) {
	Clair::Renderer::setViewport(0, 0, width, height);
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(90.0f), aspect, 0.01f, 100.0f)));
}

void DeferredSample::update() {
	mConstBuffer->Time = getRunningTime();
	Camera::update(getDeltaTime());
}

void DeferredSample::render() {
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::setCameraPosition(value_ptr(Camera::getPosition()));

	Clair::Renderer::clear(true);

	Clair::Renderer::setRenderTargetGroup(mRenderTargetGroup);
	mRenderTarget->clear({1.0f, 0.0f, 0.0f, 1.0f});
	mDepthTarget->clearDepth(1.0f);
	Clair::Renderer::render(mScene);

	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::clear(false);
	Clair::Renderer::renderScreenQuad(mDrawTexture);

	Clair::Renderer::finalizeFrame();
}