#include "DeferredSample.h"
#include "Clair/Object.h"
#include "Clair/Clair.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "SampleFramework/Random.h"
#include "ImGui/imgui.h"
#include "Clair/Material.h"
#include "../../data/materials/deferred/geometry.h"
#include "../../data/materials/deferred/composite.h"
#include "../../rawdata/materials/deferred/numLights.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

Clair::Texture* DeferredSample::createGBufferTarget(
	Clair::Texture::Format format,
	Clair::Texture::Type type) const {
	Clair::Texture* tex {Clair::ResourceManager::createTexture()};
	Clair::Texture::Options texOptions;
	texOptions.width = 960;
	texOptions.height = 640;
	texOptions.format = format;
	texOptions.type = type;
	tex->initialize(texOptions);
	return tex;
}

bool DeferredSample::initialize(const HWND hwnd) {
	if (!Clair::initialize(hwnd, Logger::log)) {
		return false;
	}
	// Create G-buffer
	//      |-------------------------------------------|
	// RT0: |             depth              | stencil  | 32 bits
	//      |-------------------------------------------|
	// RT1: |      normal.r       |      normal.g       | 32 bits
	//      |-------------------------------------------|
	// RT2: | albedo.r | albedo.g | albedo.b | emissive | 32 bits
	//      |-------------------------------------------|
	mRT0 = createGBufferTarget(
		Clair::Texture::Format::D24_UNORM_S8_UINT,
		Clair::Texture::Type::DEPTH_STENCIL_TARGET);
	mRT1 = createGBufferTarget(
		Clair::Texture::Format::R16G16_FLOAT,
		Clair::Texture::Type::RENDER_TARGET);
	mRT2 = createGBufferTarget(
		Clair::Texture::Format::R8G8B8A8_UNORM,
		Clair::Texture::Type::RENDER_TARGET);
	mGBuffer = new Clair::RenderTargetGroup{2};
	mGBuffer->setDepthStencilTarget(mRT0);
	mGBuffer->setRenderTarget(0, mRT1->getRenderTarget());
	mGBuffer->setRenderTarget(1, mRT2->getRenderTarget());

	// Deferred materials
	auto geometryMatData =
		Loader::loadBinaryData("materials/deferred/geometry.cmat");
	mGeometryMat = Clair::ResourceManager::createMaterial();
	mGeometryMat->initialize(geometryMatData.get());
	auto compositeMatData =
		Loader::loadBinaryData("materials/deferred/composite.cmat");
	Clair::Material* compositeMat {Clair::ResourceManager::createMaterial()};
	compositeMat->initialize(compositeMatData.get());
	mCompositeMatInstance = Clair::ResourceManager::createMaterialInstance();
	mCompositeMatInstance->initialize(compositeMat);
	mCompositeCBuffer = mCompositeMatInstance->
		getConstantBufferPs<Cb_materials_deferred_composite_Ps>();
	mCompositeMatInstance->setShaderResource(0, mRT0->getShaderResource());
	mCompositeMatInstance->setShaderResource(1, mRT1->getShaderResource());
	mCompositeMatInstance->setShaderResource(2, mRT2->getShaderResource());

	// Place some objects in the scene
	auto bunnyMeshData = Loader::loadBinaryData("models/bunny.cmod");
	auto bunnyMesh = Clair::ResourceManager::createMesh();
	bunnyMesh->initialize(bunnyMeshData.get());
	auto planeMeshData = Loader::loadBinaryData("models/plane.cmod");
	auto planeMesh = Clair::ResourceManager::createMesh();
	planeMesh->initialize(planeMeshData.get());
	mScene = Clair::ResourceManager::createScene();
	createObject(planeMesh, {1.0f, 1.0f, 1.0f},
				value_ptr(scale(vec3{2.0f})));
	createObject(bunnyMesh, {1.0f, 0.2f, 0.2f},
				 value_ptr(translate(vec3{-2.0f, 0.0f, 2.0f})));
	createObject(bunnyMesh, {0.2f, 1.0f, 0.2f},
				 value_ptr(translate(vec3{-2.0f, 0.0f, -2.0f})));
	createObject(bunnyMesh, {0.2f, 0.2f, 1.0f},
				 value_ptr(translate(vec3{2.0f, 0.0f, -2.0f})));
	createObject(bunnyMesh, {1.0f, 0.2f, 1.0f},
				 value_ptr(translate(vec3{2.0f, 0.0f, 2.0f})));

	mLights = new Light[NUM_LIGHTS];
	resetLights();

	// Light debug objects
	auto cubeMeshData = Loader::loadBinaryData("models/cube.cmod");
	auto cubeMesh = Clair::ResourceManager::createMesh();
	cubeMesh->initialize(cubeMeshData.get());
	mLightDebugScene = Clair::ResourceManager::createScene();
	for (int i {0}; i < NUM_LIGHTS; ++i) {
		auto& light = mLights[i];
		light.debugObj = mLightDebugScene->createObject();
		light.debugObj->setMesh(cubeMesh);
		light.debugObj->setMaterial(CLAIR_RENDER_PASS(0), mGeometryMat);
		light.debugObj->setMatrix(value_ptr(translate(vec3{light.rotationRadius,
			light.height, light.rotationRadius}) * scale(vec3{0.05f})));
	}

	Camera::initialize({0.0f, 5.5f, -4.0f}, 1.14f, 0.0f);
	return true;
}

void DeferredSample::createObject(Clair::Mesh* mesh,
								  const Clair::Float3& color,
								  const Clair::Float4x4& transform) {
	auto obj = mScene->createObject();
	obj->setMesh(mesh);
	obj->setMatrix(transform);
	auto matInstance = obj->setMaterial(CLAIR_RENDER_PASS(0), mGeometryMat);
	auto cbuf =
		matInstance->getConstantBufferPs<Cb_materials_deferred_geometry_Ps>();
	cbuf->Albedo = color;
	cbuf->Emissive = 0.0f;
}

void DeferredSample::resetLights() {
	for (int i {0}; i < NUM_LIGHTS; ++i) {
		auto& light = mLights[i];
		light.color = vec3{Random::randomFloat(), Random::randomFloat(),
					   Random::randomFloat()};
		light.intensity = Random::randomFloat(1.5f, 2.0f) /
			(0.2f * static_cast<float>(NUM_LIGHTS));
		light.height = Random::randomFloat(0.1f, 2.0f);
		light.rotationRadius = Random::randomFloat(0.8f, 5.0f);
		light.rotationSpeed = Random::randomFloat(-1.0f, 1.0f);
		light.offset = Random::randomFloat(0.0f, 2.0f * glm::pi<float>());
	}
}

void DeferredSample::terminate() {
	delete[] mLights;
	delete mGBuffer;
	Clair::terminate();
}

void DeferredSample::onResize() {
	Clair::Renderer::resizeSwapBuffer(getWidth(), getHeight());
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	mProjectionMat = perspectiveLH(radians(90.0f), getAspect(), 0.01f, 100.0f);
	Clair::Renderer::setProjectionMatrix(value_ptr(mProjectionMat));
	mRT0->resize(getWidth(), getHeight());
	mRT1->resize(getWidth(), getHeight());
	mRT2->resize(getWidth(), getHeight());
}

void DeferredSample::update() {
	Camera::update(getDeltaTime());

	// Update lights
	for (int i {0}; i < NUM_LIGHTS; ++i) {
		auto col = mLights[i].color;
		mCompositeCBuffer->LightColors[i] = {
			col.r, col.g, col.b, mLights[i].intensity };
		const float p {
			getRunningTime() * mLights[i].rotationSpeed + mLights[i].offset};
		const vec3 lightPos {
			cosf(p) * mLights[i].rotationRadius,
			cosf(p * 3.0f) * 0.2f + mLights[i].height,
			sinf(p) * mLights[i].rotationRadius
		};
		mCompositeCBuffer->LightPositions[i] = {
			lightPos.x, lightPos.y, lightPos.z, 0.0f};

		// Light debug objects
		auto obj = mLights[i].debugObj;
		obj->setMatrix(value_ptr(translate(lightPos) * scale(vec3{0.05f})));
		auto matInst = obj->getMaterial(CLAIR_RENDER_PASS(0));
		auto cbuf =
			matInst->getConstantBufferPs<Cb_materials_deferred_geometry_Ps>();
		cbuf->Albedo = {col.r, col.g, col.b};
		cbuf->Emissive = mLights[i].intensity;
	}

	// GUI
	if (ImGui::Button("Reset lights")) resetLights();
	ImGui::Checkbox("Show lights", &mDrawLightDebugCubes);
	ImGui::Checkbox("Show G-buffer", &mDrawGBuffers);
}

void DeferredSample::render() {
	const mat4 viewMat = Camera::getViewMatrix();
	// Render to G-buffer
	mRT0->clear({1.0f});
	mRT1->getRenderTarget()->clear({0.0f, 0.0f, 0.0f, 1.0f});
	mRT2->getRenderTarget()->clear({0.0f, 0.0f, 0.0f, 1.0f});
	Clair::Renderer::setRenderTargetGroup(mGBuffer);
	Clair::Renderer::setViewMatrix(value_ptr(viewMat));
	Clair::Renderer::render(mScene);
	if (mDrawLightDebugCubes) {
		Clair::Renderer::render(mLightDebugScene);
	}

	// Composite
	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::clearDepthStencil(1.0f, 0);
	mCompositeCBuffer->DrawGBuffers = mDrawGBuffers;
	mCompositeCBuffer->InverseProj = value_ptr(inverse(mProjectionMat));
	mCompositeCBuffer->View = value_ptr(viewMat);
	Clair::Renderer::renderScreenQuad(mCompositeMatInstance);
	Clair::Renderer::finalizeFrame();
}
