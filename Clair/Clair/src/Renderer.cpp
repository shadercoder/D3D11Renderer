#include "Clair/Renderer.h"
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include "Clair/Matrix.h"
#include "LowLevelRenderer.h"

using namespace Clair;

bool Renderer::initialize() {
	return true;
}

void Renderer::terminate() {
}

void Renderer::clear(const bool clearCol) {
	LowLevelRenderer::clear(clearCol);
}

void Renderer::finalizeFrame() {
	LowLevelRenderer::finalizeFrame();
}

void Renderer::setRenderPass(const RenderPass pass) {
	LowLevelRenderer::setRenderPass(pass);
}

void Renderer::setViewport(const int x, const int y,
								  const int width, const int height) {
	LowLevelRenderer::setViewport(x, y, width, height);
}

void Renderer::setViewMatrix(const Float4x4& view) {
	LowLevelRenderer::setViewMatrix(view);
}

void Renderer::setProjectionMatrix(const Float4x4& projection) {
	LowLevelRenderer::setProjectionMatrix(projection);
}

void Renderer::setCameraPosition(const Float3& position) {
	LowLevelRenderer::setCameraPosition(position);
}

void Renderer::render(Scene* const scene) {
	if (!scene) return;

	LowLevelRenderer::render(scene);
}

void Renderer::renderScreenQuad(const MaterialInstance* const materialInstance) {
	LowLevelRenderer::renderScreenQuad(materialInstance);
}

Texture* Renderer::getDefaultRenderTarget() {
	return LowLevelRenderer::getDefaultRenderTarget();
}