#include "MaterialSample.h"
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

bool MaterialSample::initialize(const HWND hwnd) {
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
	const int size = 10;
	const float fsize = static_cast<float>(size);
	for (int i = 0; i < size; ++i) {
		const float fi = static_cast<float>(i) / (fsize - 1);
		Clair::Object* const obj = mScene->createObject();
		obj->setMesh(sphereMesh);
		obj->setMatrix(value_ptr(
			translate(vec3{fi * fsize * 2.2f - fsize, 0, 0})));
		auto matInst = obj->setMaterial(CLAIR_RENDER_PASS(0), material);
		matInst->setShaderResource(0, mSkyTexture->getShaderResource());
		auto cbuf = matInst->getConstantBufferPs<Cb_materials_pbrSimple_Ps>();
		cbuf->Reflectivity = 1.0f;
		cbuf->Roughness = fi;
		cbuf->Metalness = 0.0f;
	}
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
		//0, mSkyTexture->createCustomShaderResource(0, 6, 2, 1, true));
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
		auto outFace = mSkyTexture->createCustomRenderTarget(i_face, 1, 0, 1);
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

	Camera::initialize({1.0f, 1.1f, -15.0f}, 0.265f, 0.0f);
	return true;
}

void MaterialSample::filterCubeMap() {
	int res = 256;
	for (size_t i_mip {1}; i_mip < mSkyTexture->getNumMipMaps(); ++i_mip) {
		auto renderTargets = Clair::RenderTargetGroup{6};
		for (int i_face {0}; i_face < 6; ++i_face) {
			auto outFace = mSkyTexture->createCustomRenderTarget(
				i_face, 1, i_mip, 1);
			renderTargets.setRenderTarget(i_face, outFace);
		}
		Clair::Renderer::setViewport(0, 0, res, res);
		res /= 2;
		Clair::Renderer::setRenderTargetGroup(&renderTargets);
		auto inputCube =
			//mSkyTexture->createCustomShaderResource(0, 6, 0, 1, true);
			mSkyTexture->createCustomShaderResource(0, 6, i_mip - 1, 1, true);
		mFilterCubeMapMatInstance->setShaderResource(0, inputCube);
		mFilterCubeMapCBuffer->Roughness = static_cast<float>(i_mip) /
			static_cast<float>(NUM_ROUGHNESS_MIPS - 1);
		Clair::Renderer::renderScreenQuad(mFilterCubeMapMatInstance);
	}
	Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
	Clair::Renderer::setRenderTargetGroup(nullptr);
}

void MaterialSample::terminate() {
	Clair::terminate();
}

void MaterialSample::onResize(const int width, const int height,
						   const float aspect) {
	Clair::Renderer::setViewport(0, 0, width, height);
	Clair::Renderer::resizeScreen(width, height);
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(mFoV), aspect, 0.01f, 100.0f)));
	mSkyConstBuffer->Aspect = aspect;
	mSkyConstBuffer->FieldOfView = mFoV;
}

void MaterialSample::update() {
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

void MaterialSample::render() {
	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::setCameraPosition(value_ptr(Camera::getPosition()));
	Clair::Renderer::clearColor({0,0,0,0});
	Clair::Renderer::renderScreenQuad(mSkyMaterialInstance);
	Clair::Renderer::clearDepthStencil(1.0f, 0);
	Clair::Renderer::render(mScene);
	Clair::Renderer::finalizeFrame();
}
