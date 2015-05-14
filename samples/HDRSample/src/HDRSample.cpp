#include "HDRSample.h"
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
#include "../../data/materials/pbrSimple.h"
#include "../../data/materials/filterCube.h"
#include "../../rawdata/materials/numRoughnessMips.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

bool HDRSample::initialize(const HWND hwnd) {
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
	// RT0: |              depth             | stencil  | 32 bits
	//      |-------------------------------------------|
	// RT1: |       normal.r      |       normal.g      | 32 bits
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
	mGBuffer = new Clair::RenderTargetGroup{2};
	mGBuffer->setDepthStencilTarget(RT0);
	mGBuffer->setRenderTarget(0, RT1->getRenderTarget());
	mGBuffer->setRenderTarget(1, RT2->getRenderTarget());
	auto sphereMeshData = Loader::loadBinaryData("models/sphere.cmod");
	auto sphereMesh = Clair::ResourceManager::createMesh();
	sphereMesh->initialize(sphereMeshData.get());
	
	auto bunnyMeshData = Loader::loadBinaryData("models/bunny.cmod");
	auto bunnyMesh = Clair::ResourceManager::createMesh();
	bunnyMesh->initialize(bunnyMeshData.get());

	auto matData = Loader::loadBinaryData("materials/pbrSimple.cmat");
	auto material = Clair::ResourceManager::createMaterial();
	material->initialize(matData.get());

	auto skyMatData = Loader::loadBinaryData("materials/sky.cmat");
	auto skyMaterial = Clair::ResourceManager::createMaterial();
	skyMaterial->initialize(skyMatData.get());

	mScene = Clair::ResourceManager::createScene();
	Clair::Object* const obj = mScene->createObject();
	obj->setMesh(bunnyMesh);
	obj->setMatrix(value_ptr(
		translate(vec3{1.1f, -5.0f, -5.0f}) * scale(vec3{1.0f} * 2.0f)));
	auto matInst = obj->setMaterial(CLAIR_RENDER_PASS(0), material);
	matInst->setShaderResource(0, mSkyTexture->getShaderResource());
	mModelCBuffer = matInst->getConstantBufferPs<Cb_materials_pbrSimple_Ps>();

	mSkyMaterialInstance = Clair::ResourceManager::createMaterialInstance();
	mSkyMaterialInstance->initialize(skyMaterial);
	mSkyMaterialInstance->setShaderResource(
		0, mSkyTexture->getShaderResource());
	mSkyConstBuffer =
		mSkyMaterialInstance->getConstantBufferPs<Cb_materials_sky_Ps>();

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

	// filtering the cube map
	auto filterMatData = Loader::loadBinaryData("materials/filterCube.cmat");
	auto filterMaterial = Clair::ResourceManager::createMaterial();
	filterMaterial->initialize(filterMatData.get());
	mFilterCubeMapMatInstance =
		Clair::ResourceManager::createMaterialInstance();
	mFilterCubeMapMatInstance->initialize(filterMaterial);
	mFilterCubeMapCBuffer = mFilterCubeMapMatInstance->getConstantBufferPs<
		Cb_materials_filterCube_Ps>();
	filterCubeMap();

	// creating render target
	mSceneTarget = Clair::ResourceManager::createTexture();
	Clair::Texture::Options sceneTargTexOptions;
	sceneTargTexOptions.width = 960;
	sceneTargTexOptions.height = 640;
	sceneTargTexOptions.format = Clair::Texture::Format::R32G32B32A32_FLOAT;
	sceneTargTexOptions.type = Clair::Texture::Type::RENDER_TARGET;
	mSceneTarget->initialize(texOptions);

	Camera::initialize({1.0f, 1.1f, -15.0f}, 0.265f, 0.0f);
	return true;
}

void HDRSample::filterCubeMap() {
	int w, h;
	mSkyTexture->getMipMapDimensions(1, &w, &h);
	for (size_t i_mip {1}; i_mip < NUM_ROUGHNESS_MIPS; ++i_mip) {
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
		o.isCubeMap = true;
		auto inputCube = mSkyTexture->createSubShaderResource(o);
		mFilterCubeMapMatInstance->setShaderResource(0, inputCube);
		mFilterCubeMapCBuffer->Roughness = static_cast<float>(i_mip) /
			static_cast<float>(NUM_ROUGHNESS_MIPS - 1);
		Clair::Renderer::renderScreenQuad(mFilterCubeMapMatInstance);
	}
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	Clair::Renderer::setRenderTargetGroup(nullptr);
}

Clair::Texture* HDRSample::createGBufferTarget(
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

void HDRSample::terminate() {
	Clair::terminate();
}

void HDRSample::onResize() {
	Clair::Renderer::resizeSwapBuffer(getWidth(), getHeight());
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(mFoV), getAspect(), 0.01f, 100.0f)));
	mSkyConstBuffer->Aspect = getAspect();
	mSkyConstBuffer->FieldOfView = mFoV;
}

void HDRSample::update() {
	Camera::update(getDeltaTime());
	mSkyConstBuffer->CamRight = value_ptr(Camera::getRight());
	mSkyConstBuffer->CamUp = value_ptr(Camera::getUp());
	mSkyConstBuffer->CamForward = value_ptr(Camera::getForward());
	
	ImGui::SliderFloat("Reflectivity", &mReflectivity, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &mRoughness, 0.0f, 1.0f);
	ImGui::SliderFloat("Metalness", &mMetalness, 0.0f, 1.0f);
	mModelCBuffer->Reflectivity = mReflectivity;
	mModelCBuffer->Roughness = mRoughness;
	mModelCBuffer->Metalness = mMetalness;
}

void HDRSample::render() {
	auto sceneRTGroup = Clair::RenderTargetGroup{1};
	sceneRTGroup.setRenderTarget(0, mSceneTarget->getRenderTarget());
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::setCameraPosition(value_ptr(Camera::getPosition()));
	Clair::Renderer::setRenderTargetGroup(&sceneRTGroup);
	Clair::Renderer::renderScreenQuad(mSkyMaterialInstance);
	Clair::Renderer::render(mScene);
	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::clearDepthStencil(1.0f, 0);
	Clair::Renderer::finalizeFrame();
}