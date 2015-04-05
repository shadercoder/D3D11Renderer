#include <Windows.h>
#include "Sample.h"
#include "Clair/Renderer.h"
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include "GlmMath.h"
#include "Camera.h"
#include "LoadBinaryData.h"

using namespace glm;

Clair::Scene* scene {nullptr};

bool Sample::initialize(HWND hwnd) {
	if (!Clair::Renderer::initialize(hwnd)) return false;

	scene = Clair::Renderer::createScene();

	auto meshData = loadBinaryData("../data/model.cmod");
	Clair::Renderer::createMesh(meshData.data());

	auto materialData = loadBinaryData("../data/material.cmat");
	Clair::Renderer::createMaterial(materialData.data());

	return true;
}

void Sample::terminate() {
	Clair::Renderer::terminate();
}

void Sample::update(const float deltaTime, const float ) {
	Camera::update(deltaTime);
}

void Sample::render() {
	Clair::Renderer::clear();
	Clair::Renderer::setCameraMatrix(
							Clair::Matrix(value_ptr(Camera::getViewMatrix())));
	Clair::Renderer::render(scene);
	Clair::Renderer::finalizeFrame();
}

void Sample::onResize(const int width, const int height) {
	Clair::Renderer::setViewport(0, 0, width, height);
}