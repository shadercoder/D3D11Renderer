#include "AdvancedSample.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "ImGui/imgui.h"
#include "Clair/Clair.h"
#include "Clair/Material.h"
#include "Clair/Mesh.h"
#include "Clair/Object.h"
#include "Clair/RenderTargetGroup.h"
#include "../../data/materials/filterCube.h"
#include "../../rawdata/materials/numRoughnessMips.h"
#include "../../data/materials/advanced/geometry.h"
#include "../../data/materials/advanced/composite.h"
#include "../../data/materials/advanced/filterFrame.h"
#include "../../data/materials/pbrSky.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

AdvancedSample::~AdvancedSample() {
	delete mGBuffer;
	delete mPreviousFrame;
}

bool AdvancedSample::initialize(const HWND hwnd) {
	if (!Clair::initialize(hwnd, Logger::log)) {
		return false;
	}
	mSkyTexture = Clair::ResourceManager::createTexture();
	Clair::Texture::Options texOptions {};
	texOptions.width = 512;
	texOptions.height = 512;
	texOptions.arraySize = 6;
	texOptions.isCubeMap = true;
	texOptions.maxMipLevels = NUM_ROUGHNESS_MIPS;
	texOptions.type = Clair::Texture::Type::RENDER_TARGET;
	texOptions.format = Clair::Texture::Format::R32G32B32A32_FLOAT;
	mSkyTexture->initialize(texOptions);

	// Create G-buffer
	//      |-------------------------------------------|
	// RT0: |             depth              | stencil  | 32 bits
	//      |-------------------------------------------|
	// RT1: |      normal.r       |      normal.g       | 32 bits
	//      |-------------------------------------------|
	// RT2: | albedo.r | albedo.g | albedo.b | emissive | 32 bits
	//      |-------------------------------------------|
	// RT3: |  gloss   |  metal   |    -     |    -     | 32 bits
	//      |-------------------------------------------|
	RT0 = createGBufferTarget(
		Clair::Texture::Format::D24_UNORM_S8_UINT,
		Clair::Texture::Type::DEPTH_STENCIL_TARGET);
	RT1 = createGBufferTarget(
		Clair::Texture::Format::R16G16_FLOAT,
		Clair::Texture::Type::RENDER_TARGET);
	RT2 = createGBufferTarget(
		Clair::Texture::Format::R8G8B8A8_UNORM,
		Clair::Texture::Type::RENDER_TARGET);
	RT3 = createGBufferTarget(
		Clair::Texture::Format::R8G8B8A8_UNORM,
		Clair::Texture::Type::RENDER_TARGET);
	mGBuffer = new Clair::RenderTargetGroup{3};
	mGBuffer->setDepthStencilTarget(RT0);
	mGBuffer->setRenderTarget(0, RT1->getRenderTarget());
	mGBuffer->setRenderTarget(1, RT2->getRenderTarget());
	mGBuffer->setRenderTarget(2, RT3->getRenderTarget());

	// Creating cube map from HDR image
	auto hdrSkyTexData = Loader::loadHDRImageData("textures/pisa.hdr");
	auto hdrSkyTex = Clair::ResourceManager::createTexture();
	Clair::Texture::Options hdrTexOptions {};
	hdrTexOptions.width = hdrSkyTexData.width;
	hdrTexOptions.height = hdrSkyTexData.height;
	hdrTexOptions.format = Clair::Texture::Format::R32G32B32A32_FLOAT;
	hdrTexOptions.initialData = hdrSkyTexData.data;
	hdrSkyTex->initialize(hdrTexOptions);
	auto panCubeData = Loader::loadBinaryData("materials/panoramaToCube.cmat");
	auto panCubeMat = Clair::ResourceManager::createMaterial();
	panCubeMat->initialize(panCubeData.get());
	mPanoramaToCube = Clair::ResourceManager::createMaterialInstance();
	mPanoramaToCube->initialize(panCubeMat);
	mPanoramaToCube->setShaderResource(0, hdrSkyTex->getShaderResource());
	auto renderTargets = Clair::RenderTargetGroup{6};
	for (int i_face {0}; i_face < 6; ++i_face) {
		Clair::SubTextureOptions o;
		o.arrayStartIndex = i_face;
		o.arraySize = 1;
		auto outFace = mSkyTexture->createSubRenderTarget(o);
		renderTargets.setRenderTarget(i_face, outFace);
	}
	Clair::Renderer::setViewport(0, 0, 512, 512);
	Clair::Renderer::setRenderTargetGroup(&renderTargets);
	Clair::Renderer::renderScreenQuad(mPanoramaToCube);
	Clair::Renderer::setRenderTargetGroup(nullptr);

	// Filtering the cube map
	auto filterMatData = Loader::loadBinaryData("materials/filterCube.cmat");
	auto filterMaterial = Clair::ResourceManager::createMaterial();
	filterMaterial->initialize(filterMatData.get());
	mFilterCubeMapMatInstance =
		Clair::ResourceManager::createMaterialInstance();
	mFilterCubeMapMatInstance->initialize(filterMaterial);
	mFilterCubeMapCBuffer = mFilterCubeMapMatInstance->getConstantBufferPs<
		Cb_materials_filterCube_Ps>();
	filterCubeMap();

	// Saving frame so it can be filtered and reflected using SSR
	mPreviousFrameTex = Clair::ResourceManager::createTexture();
	Clair::Texture::Options savedFrameTexOptions {};
	savedFrameTexOptions.width = 960;
	savedFrameTexOptions.height = 640;
	savedFrameTexOptions.maxMipLevels = NUM_ROUGHNESS_MIPS;
	savedFrameTexOptions.type = Clair::Texture::Type::RENDER_TARGET;
	savedFrameTexOptions.format = Clair::Texture::Format::R32G32B32A32_FLOAT;
	mPreviousFrameTex->initialize(savedFrameTexOptions);
	mPreviousFrame = new Clair::RenderTargetGroup{1};
	mPreviousFrame->setRenderTarget(0, mPreviousFrameTex->getRenderTarget());
	auto filterFrameMatData =
		Loader::loadBinaryData("materials/advanced/filterFrame.cmat");
	auto filterFrameMaterial = Clair::ResourceManager::createMaterial();
	filterFrameMaterial->initialize(filterFrameMatData.get());
	mFilterFrameMatInstance =
		Clair::ResourceManager::createMaterialInstance();
	mFilterFrameMatInstance->initialize(filterFrameMaterial);

	// Frame after deferred
	mAfterDeferredTex = Clair::ResourceManager::createTexture();
	Clair::Texture::Options afterDefTexOptions {};
	afterDefTexOptions.width = 960;
	afterDefTexOptions.height = 640;
	afterDefTexOptions.maxMipLevels = NUM_ROUGHNESS_MIPS;
	afterDefTexOptions.type = Clair::Texture::Type::RENDER_TARGET;
	afterDefTexOptions.format = Clair::Texture::Format::R32G32B32A32_FLOAT;
	mAfterDeferredTex->initialize(afterDefTexOptions);
	mAfterDeferred = new Clair::RenderTargetGroup{1};
	mAfterDeferred->setRenderTarget(0, mAfterDeferredTex->getRenderTarget());

	auto drawTexMatData =
		Loader::loadBinaryData("materials/advanced/drawTexture.cmat");
	auto drawTexMat = Clair::ResourceManager::createMaterial();
	drawTexMat->initialize(drawTexMatData.get());
	mDrawTextureMatInstance = Clair::ResourceManager::createMaterialInstance();
	mDrawTextureMatInstance->initialize(drawTexMat);
	mDrawTextureMatInstance->setShaderResource(
		0, mAfterDeferredTex->getShaderResource());

	// Material for drawing the sky
	auto skyMatData = Loader::loadBinaryData("materials/pbrSky.cmat");
	auto skyMaterial = Clair::ResourceManager::createMaterial();
	skyMaterial->initialize(skyMatData.get());
	mSkyMaterialInstance = Clair::ResourceManager::createMaterialInstance();
	mSkyMaterialInstance->initialize(skyMaterial);
	mSkyMaterialInstance->setShaderResource(
		0, mSkyTexture->getShaderResource());
	mSkyConstBuffer =
		mSkyMaterialInstance->getConstantBufferPs<Cb_materials_pbrSky_Ps>();

	// Materials
	auto sphereMeshData = Loader::loadBinaryData("models/sphere.cmod");
	auto sphereMesh = Clair::ResourceManager::createMesh();
	sphereMesh->initialize(sphereMeshData.get());
	
	auto bunnyMeshData = Loader::loadBinaryData("models/bunny.cmod");
	auto bunnyMesh = Clair::ResourceManager::createMesh();
	bunnyMesh->initialize(bunnyMeshData.get());

	auto planeMeshData = Loader::loadBinaryData("models/plane.cmod");
	auto planeMesh = Clair::ResourceManager::createMesh();
	planeMesh->initialize(planeMeshData.get());

	auto matData = Loader::loadBinaryData("materials/advanced/geometry.cmat");
	auto material = Clair::ResourceManager::createMaterial();
	material->initialize(matData.get());

	// Deferred composite material
	auto compTexData =
		Loader::loadBinaryData("materials/advanced/composite.cmat");
	auto compTex = Clair::ResourceManager::createMaterial();
	compTex->initialize(compTexData.get());
	mCompositeMat = Clair::ResourceManager::createMaterialInstance();
	mCompositeMat->initialize(compTex);
	mCompositeMat->setShaderResource(
		0, RT0->getShaderResource());
	mCompositeMat->setShaderResource(
		1, RT1->getShaderResource());
	mCompositeMat->setShaderResource(
		2, RT2->getShaderResource());
	mCompositeMat->setShaderResource(
		3, RT3->getShaderResource());
	mCompositeMat->setShaderResource(
		4, mPreviousFrameTex->getShaderResource());
	mCompositeMat->setShaderResource(
		5, mSkyTexture->getShaderResource());
	mCompositeCBuffer = mCompositeMat->
		getConstantBufferPs<Cb_materials_advanced_composite_Ps>();

	mScene = Clair::ResourceManager::createScene();
	const int size = 10;
	const float fsize = static_cast<float>(size);
	for (int i = 0; i < size; ++i) {
		const float fi = static_cast<float>(i) / (fsize - 1);
		Clair::Object* const obj = mScene->createObject();
		obj->setMesh(sphereMesh);
		obj->setMatrix(value_ptr(
			translate(vec3{fi * fsize * 2.2f - fsize, 0, 0})));
		auto matInst = obj->setMaterial(CLAIR_RENDER_PASS(0), material);
		auto cbuf =
			matInst->getConstantBufferPs<Cb_materials_advanced_geometry_Ps>();
		cbuf->Albedo = {1.0f, 1.0f, 1.0f};
		cbuf->Emissive = 0.0f;
		cbuf->Glossiness = 1.0f - fi;
		cbuf->Metalness = 1.0f;
	}
	Clair::Object* const bunny = mScene->createObject();
	bunny->setMesh(bunnyMesh);
	bunny->setMatrix(value_ptr(
		translate(vec3{1.1f, -5.05f, -5.0f}) * scale(vec3{1.0f} * 2.0f)));
	auto bunnyMatInst = bunny->setMaterial(CLAIR_RENDER_PASS(0), material);
	bunnyMatInst->setShaderResource(0, mSkyTexture->getShaderResource());
	auto cbuf =
		bunnyMatInst->getConstantBufferPs<Cb_materials_advanced_geometry_Ps>();
	cbuf->Albedo = {1.0f, 1.0f, 0.0f};
	cbuf->Emissive = 0.0f;
	cbuf->Glossiness = 0.5f;
	cbuf->Metalness = 1.0f;
	Clair::Object* const plane = mScene->createObject();
	plane->setMesh(planeMesh);
	plane->setMatrix(value_ptr(
		translate(vec3{1.1f, -5.05f, -5.0f}) * scale(vec3{1.0f} * 5.0f)));
	auto planeMatInst = plane->setMaterial(CLAIR_RENDER_PASS(0), material);
	planeMatInst->setShaderResource(0, mSkyTexture->getShaderResource());
	auto planeCbuf =
		planeMatInst->getConstantBufferPs<Cb_materials_advanced_geometry_Ps>();
	planeCbuf->Albedo = {0.0f, 1.0f, 1.0f};
	planeCbuf->Emissive = 0.0f;
	planeCbuf->Glossiness = 0.5f;
	planeCbuf->Metalness = 1.0f;
	mTweakableCbuf = planeCbuf;

	Camera::initialize({1.0f, 1.1f, -15.0f}, 0.265f, 0.0f);
	return true;
}

void AdvancedSample::filterCubeMap() {
	int w, h;
	mSkyTexture->getMipMapDimensions(1, &w, &h);
	for (size_t i_mip {1}; i_mip < mSkyTexture->getNumMipMaps(); ++i_mip) {
		auto renderTargets = Clair::RenderTargetGroup{6};
		for (int i_face {0}; i_face < 6; ++i_face) {
			Clair::SubTextureOptions o;
			o.arrayStartIndex = i_face;
			o.arraySize = 1;
			o.mipStartIndex = i_mip;
			auto outFace = mSkyTexture->createSubRenderTarget(o);
			renderTargets.setRenderTarget(i_face, outFace);
		}
		Clair::Renderer::setViewport(0, 0, w, h);
		w /= 2;
		h /= 2;
		Clair::Renderer::setRenderTargetGroup(&renderTargets);
		Clair::SubTextureOptions o;
		o.arrayStartIndex = 0;
		o.arraySize = 6;
		o.mipStartIndex = i_mip - 1; // less accurate but converges faster
		o.isCubeMap = true;
		auto inputCube = mSkyTexture->createSubShaderResource(o);
		mFilterCubeMapMatInstance->setShaderResource(0, inputCube);
		mFilterCubeMapCBuffer->Roughness = static_cast<float>(i_mip) /
			static_cast<float>(NUM_ROUGHNESS_MIPS - 1);
		Clair::Renderer::renderScreenQuad(mFilterCubeMapMatInstance);
	}
	mSkyTexture->destroyAllSubRenderTargets();
	mSkyTexture->destroyAllSubShaderResources();
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	Clair::Renderer::setRenderTargetGroup(nullptr);
}

void AdvancedSample::filterFrame() {
	int w, h;
	mPreviousFrameTex->getMipMapDimensions(0, &w, &h);
	for (size_t i_mip {0}; i_mip < mPreviousFrameTex->getNumMipMaps(); ++i_mip) {
		Clair::ShaderResource* inTex {nullptr};
		if (i_mip == 0) {
			inTex = mAfterDeferredTex->getShaderResource();
		} else {
			Clair::SubTextureOptions inTexOptions;
			inTexOptions.mipStartIndex = i_mip - 1;
			inTex = mPreviousFrameTex->createSubShaderResource(inTexOptions);
		}

		Clair::SubTextureOptions outTexOptions;
		outTexOptions.mipStartIndex = i_mip;
		auto outTex = mPreviousFrameTex->createSubRenderTarget(outTexOptions);

		mFilterFrameMatInstance->setShaderResource(0, inTex);
		auto filterFrameCbuf = mFilterFrameMatInstance->getConstantBufferPs<
			Cb_materials_advanced_filterFrame_Ps>();
		filterFrameCbuf->Roughness = static_cast<float>(i_mip) /
			static_cast<float>(NUM_ROUGHNESS_MIPS - 1);
		auto renderTargets = Clair::RenderTargetGroup{1};
		renderTargets.setRenderTarget(0, outTex);
		Clair::Renderer::setRenderTargetGroup(&renderTargets);
		Clair::Renderer::setViewport(0, 0, w, h);
		Clair::Renderer::renderScreenQuad(mFilterFrameMatInstance);
		w /= 2;
		h /= 2;
	}
	mPreviousFrameTex->destroyAllSubRenderTargets();
	mPreviousFrameTex->destroyAllSubShaderResources();
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	Clair::Renderer::setRenderTargetGroup(nullptr);
}

Clair::Texture* AdvancedSample::createGBufferTarget(
	Clair::Texture::Format format,
	Clair::Texture::Type type) const {
	auto tex = Clair::ResourceManager::createTexture();
	Clair::Texture::Options texOptions;
	texOptions.width = 960;
	texOptions.height = 640;
	texOptions.format = format;
	texOptions.type = type;
	tex->initialize(texOptions);
	return tex;
}

void AdvancedSample::terminate() {
	Clair::terminate();
}

void AdvancedSample::onResize() {
	Clair::Renderer::resizeSwapBuffer(getWidth(), getHeight());
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	mProjectionMat = perspectiveLH(radians(mFoV), getAspect(), 0.01f, 100.0f);
	Clair::Renderer::setProjectionMatrix(value_ptr(mProjectionMat));
	mSkyConstBuffer->Aspect = getAspect();
	mSkyConstBuffer->FieldOfView = mFoV;
	RT0->resize(getWidth(), getHeight());
	RT1->resize(getWidth(), getHeight());
	RT2->resize(getWidth(), getHeight());
	RT3->resize(getWidth(), getHeight());
	mPreviousFrameTex->resize(getWidth(), getHeight());
	mAfterDeferredTex->resize(getWidth(), getHeight());
}

void AdvancedSample::update() {
	Camera::update(getDeltaTime());
	mSkyConstBuffer->CamRight = value_ptr(Camera::getRight());
	mSkyConstBuffer->CamUp = value_ptr(Camera::getUp());
	mSkyConstBuffer->CamForward = value_ptr(Camera::getForward());
	
	ImGui::SliderFloat("Glossiness", &mGlossiness, 0.0f, 1.0f);
	ImGui::SliderFloat("Metalness", &mMetalness, 0.0f, 1.0f);
	mTweakableCbuf->Glossiness = mGlossiness;
	mTweakableCbuf->Metalness = mMetalness;
}

void AdvancedSample::render() {
	// Render to G-buffer
	Clair::Renderer::setRenderTargetGroup(mGBuffer);
	RT0->clear({1.0f});
	RT1->getRenderTarget()->clear({0.0f, 0.0f, 0.0f, 1.0f});
	RT2->getRenderTarget()->clear({0.0f, 0.0f, 0.0f, 1.0f});
	RT3->getRenderTarget()->clear({0.0f, 0.0f, 0.0f, 1.0f});
	Clair::Renderer::renderScreenQuad(mSkyMaterialInstance);
	RT0->clear({1.0f});
	glm::mat4 viewMat = Camera::getViewMatrix();
	Clair::Renderer::setViewMatrix(value_ptr(viewMat));
	Clair::Renderer::setCameraPosition(value_ptr(Camera::getPosition()));
	Clair::Renderer::render(mScene);

	// Deferred composite pass; renders to buffer
	Clair::Renderer::setRenderTargetGroup(mAfterDeferred);
	mCompositeCBuffer->InverseView = value_ptr(inverse(viewMat));
	mCompositeCBuffer->InverseProj = value_ptr(inverse(mProjectionMat));
	mCompositeCBuffer->Proj = value_ptr(mProjectionMat);
	Clair::Renderer::renderScreenQuad(mCompositeMat);

	// Filter frame for next frame's reflections; post process
	filterFrame();
	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::clearDepthStencil(1.0f, 0);
	Clair::Renderer::renderScreenQuad(mDrawTextureMatInstance);
	Clair::Renderer::finalizeFrame();
}