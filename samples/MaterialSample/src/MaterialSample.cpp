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
	//auto loadedTex = Loader::loadCubeImageData("textures/sky");
	mSkyTexture = Clair::ResourceManager::createTexture();
	Clair::Texture::Options texOptions {};
	texOptions.width = 512;
	texOptions.height = 512;
	texOptions.format = Clair::Texture::Format::R32G32B32A32_FLOAT;
	texOptions.initialData = nullptr;//loadedTex.data;
	texOptions.arraySize = 6;
	texOptions.type = Clair::Texture::Type::RENDER_TARGET;
	texOptions.isCubeMap = true;
	texOptions.maxMipLevels = NUM_ROUGHNESS_MIPS;
	mSkyTexture->initialize(texOptions);

	auto sphereMeshData = Loader::loadBinaryData("models/sphere.cmod");
	auto sphereMesh = Clair::ResourceManager::createMesh();
	sphereMesh->initialize(sphereMeshData.get());

	auto matData = Loader::loadBinaryData("materials/pbrSimple.cmat");
	auto material = Clair::ResourceManager::createMaterial();
	material->initialize(matData.get());

	auto skyMatData = Loader::loadBinaryData("materials/sky.cmat");
	auto skyMaterial = Clair::ResourceManager::createMaterial();
	skyMaterial->initialize(skyMatData.get());

	auto filterMatData = Loader::loadBinaryData("materials/filterCube.cmat");
	auto filterMaterial = Clair::ResourceManager::createMaterial();
	filterMaterial->initialize(filterMatData.get());

	mScene = Clair::ResourceManager::createScene();
	const int size = 10;
	const float fsize = static_cast<float>(size);
	for (int i = 0; i < size; ++i) {
		const float fi = static_cast<float>(i) / (fsize - 1);
		Clair::Object* const obj = mScene->createObject();
		obj->setMesh(sphereMesh);
		obj->setMatrix(value_ptr(translate(vec3{fi, 0, 0} * fsize * 2.2f)));
		auto matInst = obj->setMaterial(CLAIR_RENDER_PASS(0), material);
		matInst->setShaderResource(0, mSkyTexture->getShaderResource());
		auto cbuf = matInst->getConstantBufferPs<Cb_materials_pbrSimple_Ps>();
		cbuf->Reflectivity = 1.0f;
		cbuf->Roughness = fi;
		cbuf->Metalness = 0.0f;
	}

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

	// filtering the cube map
	mFilterCubeMapMatInstance =
		Clair::ResourceManager::createMaterialInstance();
	mFilterCubeMapMatInstance->initialize(filterMaterial);
	mFilterCubeMapCBuffer = mFilterCubeMapMatInstance->getConstantBufferPs<
		Cb_materials_filterCube_Ps>();
	filterCubeMap();

	Camera::initialize({-4.5f, 16.6f, -4.5f}, 0.705f, 0.770f);
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
			mSkyTexture->createCustomShaderResource(0, 6, 0, 1, true);
		//auto inputCube =
		//	mSkyTexture->createCustomShaderResource(0, 6, i_mip - 1, 1, true);
		mFilterCubeMapMatInstance->setShaderResource(0, inputCube);
		mFilterCubeMapCBuffer->Roughness = static_cast<float>(i_mip) /
			static_cast<float>(NUM_ROUGHNESS_MIPS - 1);
		Clair::Renderer::renderScreenQuad(mFilterCubeMapMatInstance);
	}
}

void MaterialSample::terminate() {
	Clair::terminate();
}

float FoV {60.0f};

void MaterialSample::onResize(const int width, const int height,
						   const float aspect) {
	Clair::Renderer::setViewport(0, 0, width, height);
	Clair::Renderer::resizeScreen(width, height);
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(FoV), aspect, 0.01f, 100.0f)));
	mSkyConstBuffer->Aspect = aspect;
	mSkyConstBuffer->FieldOfView = FoV;
}

void MaterialSample::update() {
	Camera::update(getDeltaTime());
	mSkyConstBuffer->CamRight = value_ptr(Camera::getRight());
	mSkyConstBuffer->CamUp = value_ptr(Camera::getUp());
	mSkyConstBuffer->CamForward = value_ptr(Camera::getForward());
	if (ImGui::Button("Filter cube map")) {
		filterCubeMap();
		Clair::Renderer::setRenderTargetGroup(nullptr);
	}
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
