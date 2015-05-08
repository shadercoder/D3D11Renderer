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
//#include "../../data/materials/filterCube.h"
//#include "vld.h"

using namespace SampleFramework;
using namespace glm;

bool MaterialSample::initialize(const HWND hwnd) {
	if (!Clair::initialize(hwnd, Logger::log)) {
		return false;
	}
	auto loadedTex = Loader::loadCubeImageData("textures/sky");
	mSkyTexture = Clair::ResourceManager::createTexture();
	Clair::Texture::Options texOptions {};
	texOptions.width = loadedTex.width;
	texOptions.height = loadedTex.height;
	texOptions.format = Clair::Texture::Format::R8G8B8A8_UNORM;
	texOptions.initialData = loadedTex.data;
	texOptions.arraySize = 6;
	texOptions.type = Clair::Texture::Type::RENDER_TARGET;
	texOptions.isCubeMap = true;
	texOptions.maxMipLevels = 2;
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

	auto inputCube = mSkyTexture->createCustomShaderResource(0, 6, 0, 1, true);
	mScene = Clair::ResourceManager::createScene();
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
		matInst->setShaderResource(0, inputCube);
		auto cbuf = matInst->getConstantBufferPs<Cb_materials_pbrSimple_Ps>();
		cbuf->Reflectivity = fx;
		cbuf->Roughness = fy;
		cbuf->Metalness = fz;
	}}}

	mSkyMaterialInstance = Clair::ResourceManager::createMaterialInstance();
	mSkyMaterialInstance->initialize(skyMaterial);
	mSkyMaterialInstance->setShaderResource(
		0, mSkyTexture->getShaderResource());
	mSkyConstBuffer =
		mSkyMaterialInstance->getConstantBufferPs<Cb_materials_sky_Ps>();

	// filtering the cube map
	mFilterCubeMapMatInstance =
		Clair::ResourceManager::createMaterialInstance();
	mFilterCubeMapMatInstance->initialize(skyMaterial);
	//auto inputCube = mSkyTexture->createCustomRenderTarget(0, 6, 0, 1, true);
	//mFilterCubeMapMatInstance->setTexture(0, inputCube);
	//mFilterCubeMapCBuffer = mFilterCubeMapMatInstance->getConstantBufferPs<
	//	Cb_materials_filterCube_Ps>();

	Camera::initialize({-4.5f, 16.6f, -4.5f}, 0.705f, 0.770f);
	return true;
}

void MaterialSample::filterCubeMap() {
	//for (size_t i_mip {1}; i_mip < mSkyTexture->getNumMipMaps(); ++i_mip) {
	//	auto renderTargets = Clair::RenderTargetGroup{6};
	//	for (int i_face {0}; i_face < 6; ++i_face) {
	//		auto outFace = mSkyTexture->createSubTexture(i_face, 1, i_mip, 1);
	//		renderTargets.setRenderTarget(i_face, outFace);
	//	}
	//	// depth
	//	auto depthTarget = Clair::ResourceManager::createTexture();
	//	Clair::Texture::Options depthTexOptions;
	//	depthTexOptions.width = 256;
	//	depthTexOptions.height = 256;
	//	depthTexOptions.format = Clair::Texture::Format::D24_UNORM_S8_UINT;
	//	depthTexOptions.type = Clair::Texture::Type::DEPTH_STENCIL_TARGET;
	//	depthTarget->initialize(depthTexOptions);
	//	depthTarget->clear(1.0f);
	//	renderTargets.setDepthStencilTarget(depthTarget);
	//	Clair::Renderer::setViewport(0, 0, 256, 256);
	//	Clair::Renderer::setRenderTargetGroup(&renderTargets);
	//	Clair::Renderer::renderScreenQuad(mFilterCubeMapMatInstance);
	//}
}

void MaterialSample::terminate() {
	Clair::terminate();
}

void MaterialSample::onResize(const int width, const int height,
						   const float aspect) {
	Clair::Renderer::setViewport(0, 0, width, height);
	Clair::Renderer::resizeScreen(width, height);
	Clair::Renderer::setProjectionMatrix(
		value_ptr(perspectiveLH(radians(90.0f), aspect, 0.01f, 100.0f)));
	mSkyConstBuffer->Aspect = aspect;
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
	Clair::Renderer::setViewport(0, 0, 960, 640);
	Clair::Renderer::setRenderTargetGroup(nullptr);
	Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
	Clair::Renderer::setCameraPosition(value_ptr(Camera::getPosition()));
	Clair::Renderer::clearColor({0,0,0,0});
	Clair::Renderer::renderScreenQuad(mSkyMaterialInstance);
	Clair::Renderer::clearDepthStencil(1.0f, 0);
	Clair::Renderer::render(mScene);
	Clair::Renderer::finalizeFrame();
}
