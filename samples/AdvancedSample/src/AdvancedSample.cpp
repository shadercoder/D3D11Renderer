// Models and textures:
// "Stanford Bunny"
// http://graphics.stanford.edu/~mdfisher/Data/Meshes/bunny.obj
// "Cerberus" - Andrew Maximov
// http://artisaverb.info/Cerberus.html
// Some HDR probes from the following sites:
// http://www.hdrlabs.com/sibl/archive.html
// http://gl.ict.usc.edu/Data/HighResProbes/

#include "AdvancedSample.h"
#include "SampleFramework/GlmMath.h"
#include "SampleFramework/Camera.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Logger.h"
#include "SampleFramework/Random.h"
#include "ImGui/imgui.h"
#include "Clair/Clair.h"
#include "Clair/Material.h"
#include "Clair/Mesh.h"
#include "Clair/Object.h"
#include "Clair/RenderTargetGroup.h"
#include "../../data/materials/filterCube.h"
#include "../../rawdata/materials/numRoughnessMips.h"
#include "../../data/materials/advanced/geometry.h"
#include "../../data/materials/advanced/geometryTextured.h"
#include "../../data/materials/advanced/composite.h"
#include "../../data/materials/advanced/filterReflection.h"
#include "../../data/materials/advanced/ssr.h"
#include "../../data/materials/pbrSky.h"
#include "../../rawdata/materials/advanced/numLights.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

static const char* gHdrImageNames[] {
	"mill",
	"bridge",
	"pisa",
	"lake",
	"stadium"
};

AdvancedSample::~AdvancedSample() {
	delete mGBuffer;
	delete mPostProcessBuffer[0];
	delete mPostProcessBuffer[1];
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

	// Filtering the cube map
	auto filterMatData = Loader::loadBinaryData("materials/filterCube.cmat");
	auto filterMaterial = Clair::ResourceManager::createMaterial();
	filterMaterial->initialize(filterMatData.get());
	mFilterCubeMapMatInstance =
		Clair::ResourceManager::createMaterialInstance();
	mFilterCubeMapMatInstance->initialize(filterMaterial);
	mFilterCubeMapCBuffer = mFilterCubeMapMatInstance->getConstantBufferPs<
		Cb_materials_filterCube_Ps>();

	// Creating cube map from HDR image
	mHdrSkyTex = Clair::ResourceManager::createTexture();
	auto panCubeData = Loader::loadBinaryData("materials/panoramaToCube.cmat");
	auto panCubeMat = Clair::ResourceManager::createMaterial();
	panCubeMat->initialize(panCubeData.get());
	mPanoramaToCube = Clair::ResourceManager::createMaterialInstance();
	mPanoramaToCube->initialize(panCubeMat);
	mPanoramaToCube->setShaderResource(0, mHdrSkyTex->getShaderResource());
	loadAndSetNewCubeMap(gHdrImageNames[mEnvironmentMap]);

	// Post process textures
	Clair::Texture::Options postProcTexOptions {};
	postProcTexOptions.width = 960;
	postProcTexOptions.height = 640;
	postProcTexOptions.type = Clair::Texture::Type::RENDER_TARGET;
	postProcTexOptions.format = Clair::Texture::Format::R8G8B8A8_UNORM;
	for (int i {0}; i < 2; ++i) {
		mPostProcessTex[i] = Clair::ResourceManager::createTexture();
		mPostProcessTex[i]->initialize(postProcTexOptions);
		mPostProcessBuffer[i] = new Clair::RenderTargetGroup{1};
		mPostProcessBuffer[i]->setRenderTarget(
			0, mPostProcessTex[i]->getRenderTarget());
	}

	mReflectionTex = Clair::ResourceManager::createTexture();
	Clair::Texture::Options afterDefTexOptions {};
	afterDefTexOptions.width = 960;
	afterDefTexOptions.height = 640;
	afterDefTexOptions.maxMipLevels = NUM_ROUGHNESS_MIPS;
	afterDefTexOptions.type = Clair::Texture::Type::RENDER_TARGET;
	afterDefTexOptions.format = Clair::Texture::Format::R8G8B8A8_UNORM;
	mReflectionTex->initialize(afterDefTexOptions);
	mReflectionBuffer = new Clair::RenderTargetGroup{1};
	mReflectionBuffer->setRenderTarget(0, mReflectionTex->getRenderTarget());
	auto reflectionMatData =
		Loader::loadBinaryData("materials/advanced/ssr.cmat");
	auto reflectionMat = Clair::ResourceManager::createMaterial();
	reflectionMat->initialize(reflectionMatData.get());
	mReflectionMatInstance =
		Clair::ResourceManager::createMaterialInstance();
	mReflectionMatInstance->initialize(reflectionMat);
	mReflectionMatInstance->setShaderResource(
		0, RT0->getShaderResource());
	mReflectionMatInstance->setShaderResource(
		1, RT1->getShaderResource());
	mReflectionCbuffer = mReflectionMatInstance->
		getConstantBufferPs<Cb_materials_advanced_ssr_Ps>();
	auto filterReflectionMatData =
		Loader::loadBinaryData("materials/advanced/filterReflection.cmat");
	auto filterReflectionMat = Clair::ResourceManager::createMaterial();
	filterReflectionMat->initialize(filterReflectionMatData.get());
	mFilterReflectionMatInstance =
		Clair::ResourceManager::createMaterialInstance();
	mFilterReflectionMatInstance->initialize(filterReflectionMat);

	// Drawing final buffer to screen
	auto drawTexMatData =
		Loader::loadBinaryData("materials/advanced/drawTexture.cmat");
	auto drawTexMat = Clair::ResourceManager::createMaterial();
	drawTexMat->initialize(drawTexMatData.get());
	mDrawTextureMatInstance = Clair::ResourceManager::createMaterialInstance();
	mDrawTextureMatInstance->initialize(drawTexMat);

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

	// Models
	auto gunMeshData = Loader::loadBinaryData("models/cerberus2.cmod");
	auto gunMesh = Clair::ResourceManager::createMesh();
	gunMesh->initialize(gunMeshData.get());

	auto planeMeshData = Loader::loadBinaryData("models/plane.cmod");
	auto planeMesh = Clair::ResourceManager::createMesh();
	planeMesh->initialize(planeMeshData.get());

	// Materials
	auto geometryMatData =
		Loader::loadBinaryData("materials/advanced/geometry.cmat");
	auto geometryMat = Clair::ResourceManager::createMaterial();
	geometryMat->initialize(geometryMatData.get());
	auto gunMatData =
		Loader::loadBinaryData("materials/advanced/geometryTextured.cmat");
	auto gunMat = Clair::ResourceManager::createMaterial();
	gunMat->initialize(gunMatData.get());

	// Cerberus gun model
	auto gunAlbedoMetalData = Loader::loadImageData("textures/cerberus_a_m.png");
	auto gunAMTex = Clair::ResourceManager::createTexture();
	Clair::Texture::Options gunAMTexOptions {};
	gunAMTexOptions.width = gunAlbedoMetalData.width;
	gunAMTexOptions.height = gunAlbedoMetalData.height;
	gunAMTexOptions.format = Clair::Texture::Format::R8G8B8A8_UNORM;
	gunAMTexOptions.initialData = gunAlbedoMetalData.data;
	gunAMTexOptions.generateMips = true;
	gunAMTex->initialize(gunAMTexOptions);
	auto gunNormalGlossData = Loader::loadImageData("textures/cerberus_n_g.png");
	auto gunNGTex = Clair::ResourceManager::createTexture();
	Clair::Texture::Options gunNGTexOptions {};
	gunNGTexOptions.width = gunNormalGlossData.width;
	gunNGTexOptions.height = gunNormalGlossData.height;
	gunNGTexOptions.format = Clair::Texture::Format::R8G8B8A8_UNORM;
	gunNGTexOptions.initialData = gunNormalGlossData.data;
	gunNGTexOptions.generateMips = true;
	gunNGTex->initialize(gunNGTexOptions);

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
		4, mReflectionTex->getShaderResource());
	mCompositeMat->setShaderResource(
		5, mSkyTexture->getShaderResource());
	mCompositeCBuffer = mCompositeMat->
		getConstantBufferPs<Cb_materials_advanced_composite_Ps>();

	mScene = Clair::ResourceManager::createScene();
	Clair::Object* const gun = mScene->createObject();
	gun->setMesh(gunMesh);
	gun->setMatrix(value_ptr(
		translate(vec3{0.0f, 3.0f, -1.0f}) *
		rotate(pi<float>() / 2.0f, vec3(0,1,0)) *
		scale(vec3{6.0f})));
	auto gunMatInst = gun->setMaterial(CLAIR_RENDER_PASS(0), gunMat);
	gunMatInst->setShaderResource(0, gunAMTex->getShaderResource());
	gunMatInst->setShaderResource(1, gunNGTex->getShaderResource());
	auto cbuf =
		gunMatInst->getConstantBufferPs<Cb_materials_advanced_geometryTextured_Ps>();
	cbuf->Emissive = 0.0f;
	Clair::Object* const plane = mScene->createObject();
	plane->setMesh(planeMesh);
	plane->setMatrix(value_ptr(
		translate(vec3{0.0f}) * scale(vec3{5.0f})));
	auto planeMatInst = plane->setMaterial(CLAIR_RENDER_PASS(0), geometryMat);
	planeMatInst->setShaderResource(0, mSkyTexture->getShaderResource());
	auto planeCbuf =
		planeMatInst->getConstantBufferPs<Cb_materials_advanced_geometry_Ps>();
	planeCbuf->Albedo = {0.8f, 0.8f, 0.8f};
	planeCbuf->Emissive = 0.0f;
	planeCbuf->Glossiness = 0.5f;
	planeCbuf->Metalness = 1.0f;
	mTweakableCbuf = planeCbuf;

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
		light.debugObj->setMaterial(CLAIR_RENDER_PASS(0), geometryMat);
		light.debugObj->setMatrix(value_ptr(translate(vec3{light.rotationRadius,
			light.height, light.rotationRadius}) * scale(vec3{0.05f})));
	}

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

void AdvancedSample::loadAndSetNewCubeMap(const std::string& filename) {
	auto hdrSkyTexData =
		Loader::loadHDRImageData("textures/" + filename + ".hdr");
	Clair::Texture::Options hdrTexOptions {};
	hdrTexOptions.width = hdrSkyTexData.width;
	hdrTexOptions.height = hdrSkyTexData.height;
	hdrTexOptions.format = Clair::Texture::Format::R32G32B32A32_FLOAT;
	hdrTexOptions.initialData = hdrSkyTexData.data;
	mHdrSkyTex->initialize(hdrTexOptions);
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

	filterCubeMap();
}

void AdvancedSample::filterReflectionBuffer() {
	int w, h;
	mReflectionTex->getMipMapDimensions(1, &w, &h);
	auto filterReflCbuf = mFilterReflectionMatInstance->getConstantBufferPs<
			Cb_materials_advanced_filterReflection_Ps>();
	filterReflCbuf->CamRight = value_ptr(Camera::getRight());
	filterReflCbuf->CamUp = value_ptr(Camera::getUp());
	filterReflCbuf->CamForward = value_ptr(Camera::getForward());
	filterReflCbuf->ViewProj =
		value_ptr(mProjectionMat * Camera::getViewMatrix());
	filterReflCbuf->Aspect = getAspect();
	filterReflCbuf->FieldOfView = mFoV;

	for (size_t i_mip {1}; i_mip < mReflectionTex->getNumMipMaps(); ++i_mip) {
		Clair::SubTextureOptions inTexOptions;
		inTexOptions.mipStartIndex = i_mip - 1;
		auto inTex = mReflectionTex->createSubShaderResource(inTexOptions);

		Clair::SubTextureOptions outTexOptions;
		outTexOptions.mipStartIndex = i_mip;
		auto outTex = mReflectionTex->createSubRenderTarget(outTexOptions);

		mFilterReflectionMatInstance->setShaderResource(0, inTex);
		filterReflCbuf->Roughness = static_cast<float>(i_mip) /
			static_cast<float>(NUM_ROUGHNESS_MIPS - 1);
		auto renderTargets = Clair::RenderTargetGroup{1};
		renderTargets.setRenderTarget(0, outTex);
		Clair::Renderer::setRenderTargetGroup(&renderTargets);
		Clair::Renderer::setViewport(0, 0, w, h);
		Clair::Renderer::renderScreenQuad(mFilterReflectionMatInstance);
		w /= 2;
		h /= 2;
	}
	mReflectionTex->destroyAllSubRenderTargets();
	mReflectionTex->destroyAllSubShaderResources();
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	Clair::Renderer::setRenderTargetGroup(nullptr);
}

void AdvancedSample::resetLights() {
	for (int i {0}; i < NUM_LIGHTS; ++i) {
		auto& light = mLights[i];
		light.color = vec3{Random::randomFloat(), Random::randomFloat(),
					   Random::randomFloat()};
		light.intensity = Random::randomFloat(3.0f, 10.0f) /
			(0.2f * static_cast<float>(NUM_LIGHTS));
		light.height = Random::randomFloat(1.0f, 5.0f);
		light.rotationRadius = Random::randomFloat(1.0f, 14.0f);
		light.rotationSpeed = Random::randomFloat(-1.0f, 1.0f);
		light.offset = Random::randomFloat(0.0f, 2.0f * glm::pi<float>());
	}
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
	mPostProcessTex[0]->resize(getWidth(), getHeight());
	mPostProcessTex[1]->resize(getWidth(), getHeight());
	mReflectionTex->resize(getWidth(), getHeight());
}

void AdvancedSample::update() {
	Camera::update(getDeltaTime());
	mSkyConstBuffer->CamRight = value_ptr(Camera::getRight());
	mSkyConstBuffer->CamUp = value_ptr(Camera::getUp());
	mSkyConstBuffer->CamForward = value_ptr(Camera::getForward());

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
			matInst->getConstantBufferPs<Cb_materials_advanced_geometry_Ps>();
		cbuf->Albedo = {col.r, col.g, col.b};
		cbuf->Emissive = mLights[i].intensity;
	}
	
	if (ImGui::Combo("Sky", &mEnvironmentMap, gHdrImageNames, 5)) {
		loadAndSetNewCubeMap(gHdrImageNames[mEnvironmentMap]);
	}
	ImGui::SliderFloat("Gloss", &mGlossiness, 0.0f, 1.0f);
	ImGui::SliderFloat("Metal", &mMetalness, 0.0f, 1.0f);
	ImGui::Checkbox("Screen-space reflections", &mSSREnabled);
	ImGui::SliderFloat("Ambient", &mAmbient, 0.0f, 5.0f);
	ImGui::Checkbox("Lights enabled", &mLightsEnabled);
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
	Clair::Renderer::render(mScene);
	if (mDrawLightDebugCubes && mLightsEnabled) {
		Clair::Renderer::render(mLightDebugScene);
	}

	// Deferred composite pass; renders to buffer
	Clair::Renderer::setRenderTargetGroup(mPostProcessBuffer[0]);
	mCompositeCBuffer->InverseView = value_ptr(inverse(viewMat));
	mCompositeCBuffer->InverseProj = value_ptr(inverse(mProjectionMat));
	mCompositeCBuffer->View = value_ptr(viewMat);
	mCompositeCBuffer->Ambient = mAmbient;
	mCompositeCBuffer->SSREnabled = mSSREnabled;
	mCompositeCBuffer->LightsEnabled = mLightsEnabled;
	Clair::Renderer::renderScreenQuad(mCompositeMat);

	// Reflections
	if (mSSREnabled) {
		Clair::Renderer::setRenderTargetGroup(mReflectionBuffer);
		mReflectionCbuffer->InverseView = value_ptr(inverse(viewMat));
		mReflectionCbuffer->InverseProj = value_ptr(inverse(mProjectionMat));
		mReflectionCbuffer->Proj = value_ptr(mProjectionMat);
		mReflectionMatInstance->setShaderResource(
			2, mPostProcessTex[0]->getShaderResource());
		Clair::Renderer::renderScreenQuad(mReflectionMatInstance);
		filterReflectionBuffer();
	}

	// Post process
	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::clearDepthStencil(1.0f, 0);
	mDrawTextureMatInstance->setShaderResource(
		0, mPostProcessTex[0]->getShaderResource());
	//	0, mReflectionTex->getShaderResource());
	Clair::Renderer::renderScreenQuad(mDrawTextureMatInstance);
	Clair::Renderer::finalizeFrame();
}
