#include "DeferredSample.h"
#include "Clair/Object.h"
#include "Clair/Clair.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "SampleFramework/Random.h"
#include "SampleFramework/Input.h"
#include "ImGui/imgui.h"
#include "Clair/Material.h"
#include "../../data/materials/deferredGeometry.h"
#include "../../data/materials/deferredComposite.h"
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

void DeferredSample::createObject(Clair::Mesh* mesh,
								  const Clair::Float4& color,
								  const Clair::Float4x4& transform) {
	auto obj = mScene->createObject();
	obj->setMesh(mesh);
	obj->setMatrix(transform);
	auto matInstance = obj->setMaterial(CLAIR_RENDER_PASS(0), mGeometryMat);
	mGeometryCBuffer =
		matInstance->getConstantBufferPs<Cb_materials_deferredGeometry_Ps>();
	mGeometryCBuffer->DiffuseColor = color;
}

void DeferredSample::resetLights() {
	for (auto& light : mLights) {
		light.color = vec3{Random::randomFloat(), Random::randomFloat(),
					   Random::randomFloat()};
		light.intensity = Random::randomFloat(0.5f, 1.0f) / 20.0f;
		light.height = Random::randomFloat(0.1f, 2.0f);
		light.rotationRadius = Random::randomFloat(0.8f, 5.0f);
		light.rotationSpeed = Random::randomFloat(-1.0f, 1.0f);
		light.offset = Random::randomFloat(0.0f, 2.0f * glm::pi<float>());
	}
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

	// Deferred composite material
	auto compTexData = Loader::loadBinaryData("materials/deferredComposite.cmat");
	auto compTex = Clair::ResourceManager::createMaterial();
	compTex->initialize(compTexData.get());
	mDeferredCompositeMat = Clair::ResourceManager::createMaterialInstance();
	mDeferredCompositeMat->initialize(compTex);
	mDeferredCompositeMat->setTexture(0, mGBufAlbedoTex);
	mDeferredCompositeMat->setTexture(1, mGBufNormalTex);
	mDeferredCompositeMat->setTexture(2, mGBufPositionTex);
	mCompositeCBuffer = mDeferredCompositeMat->
		getConstantBufferPs<Cb_materials_deferredComposite_Ps>();

	// Place some objects in the scene
	auto bunnyMeshData = Loader::loadBinaryData("models/bunny.cmod");
	auto bunnyMesh = Clair::ResourceManager::createMesh();
	bunnyMesh->initialize(bunnyMeshData.get());
	auto planeMeshData = Loader::loadBinaryData("models/plane.cmod");
	auto planeMesh = Clair::ResourceManager::createMesh();
	planeMesh->initialize(planeMeshData.get());
	auto geometryMatData =
		Loader::loadBinaryData("materials/deferredGeometry.cmat");
	mGeometryMat = Clair::ResourceManager::createMaterial();
	mGeometryMat->initialize(geometryMatData.get());
	mScene = Clair::ResourceManager::createScene();
	createObject(planeMesh, {1.0f, 1.0f, 1.0f, 1.0f},
				 value_ptr(scale(vec3{2.0f})));
	createObject(bunnyMesh, {1.0f, 0.2f, 0.2f, 1.0f},
				 value_ptr(translate(vec3{-2.0f, 0.0f, 2.0f})));
	createObject(bunnyMesh, {0.2f, 1.0f, 0.2f, 1.0f},
				 value_ptr(translate(vec3{-2.0f, 0.0f, -2.0f})));
	createObject(bunnyMesh, {0.2f, 0.2f, 1.0f, 1.0f},
				 value_ptr(translate(vec3{2.0f, 0.0f, -2.0f})));
	createObject(bunnyMesh, {1.0f, 0.2f, 1.0f, 1.0f},
				 value_ptr(translate(vec3{2.0f, 0.0f, 2.0f})));

	resetLights();

	auto cubeMeshData = Loader::loadBinaryData("models/cube.cmod");
	auto cubeMesh = Clair::ResourceManager::createMesh();
	cubeMesh->initialize(cubeMeshData.get());
	mLightDebugScene = Clair::ResourceManager::createScene();
	for (auto& light : mLights) {
		light.debugObj = mLightDebugScene->createObject();
		light.debugObj->setMesh(cubeMesh);
		light.debugObj->setMaterial(CLAIR_RENDER_PASS(0), mGeometryMat);
		light.debugObj->setMatrix(value_ptr(translate(vec3{light.rotationRadius,
			light.height, light.rotationRadius}) * scale(vec3{0.05f})));
	}

	Camera::initialize({0.0f, 5.5f, -4.0f}, 1.14f, 0.0f);
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

	for (int i {0}; i < NUM_LIGHTS; ++i) {
		auto col = mLights[i].color;
		mCompositeCBuffer->LightDiffuseColors[i] = {
			col.r, col.g, col.b, mLights[i].intensity };
		const float p {
			getRunningTime() * mLights[i].rotationSpeed + mLights[i].offset};
		const vec3 lightPos {
			cosf(p) * mLights[i].rotationRadius,
			mLights[i].height,
			sinf(p) * mLights[i].rotationRadius
		};
		mCompositeCBuffer->LightPositions[i] = {
			lightPos.x, lightPos.y, lightPos.z, 0.0f};

		// Light debug objects
		auto obj = mLights[i].debugObj;
		obj->setMatrix(value_ptr(translate(lightPos) * scale(vec3{0.05f})));
		auto matInst = obj->getMaterial(CLAIR_RENDER_PASS(0));
		mGeometryCBuffer =
			matInst->getConstantBufferPs<Cb_materials_deferredGeometry_Ps>();
		col *= mLights[i].intensity * 500.0;
		mGeometryCBuffer->DiffuseColor = {col.r, col.g, col.b, 0.0};
	}
	if (ImGui::Button("Reset lights")) resetLights();
	ImGui::Checkbox("Show lights", &mDrawLightDebugCubes);
	ImGui::Checkbox("Show G-buffers", &mDrawGBuffers);
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
	if (mDrawLightDebugCubes) {
		Clair::Renderer::render(mLightDebugScene);
	}

	// Composite
	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::clear(true);
	mCompositeCBuffer->DrawGBuffers = mDrawGBuffers;
	Clair::Renderer::renderScreenQuad(mDeferredCompositeMat);

	Clair::Renderer::finalizeFrame();
}
