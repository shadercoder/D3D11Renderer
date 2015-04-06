#include "Clair/Object.h"
#include "Material.h"
#include "LowLevelRenderer.h"
#include "Mesh.h"

using namespace Clair;

Object::~Object() {
	for (const auto& it : mMatInstances) {
		delete it.second;
	}
	LowLevelRenderer::destroyInputLayout(mInputLayout);
}

void Object::setMesh(Mesh* const mesh) {
	mMesh = mesh;
	for (const auto& instance : mMatInstances) {
		recreateInputLayout(instance.second);
	}
}

void Object::setMaterial(const RenderPass pass,
						 Material* const material) {
	MaterialInstance* instance {nullptr};
	if (mMatInstances.count(pass) == 0) {
		mMatInstances[pass] = new MaterialInstance {};
		instance = mMatInstances[pass];
	} else {
		instance = mMatInstances[pass];
		instance->isGood = false;
	}
	instance->material = material;
	recreateInputLayout(instance);
}

Material* Object::getMaterial(const RenderPass pass) {
	if (mMatInstances.count(pass) == 0) {
		return nullptr;
	}
	auto const material = mMatInstances[pass];
	if (!material->isGood) return nullptr;
	return material->material;
}

void Object::recreateInputLayout(MaterialInstance* const materialInstance) {
	bool isCompatible {true};
	if (isCompatible) {
		materialInstance->isGood = true;
		if (mInputLayout) {
			LowLevelRenderer::destroyInputLayout(mInputLayout);
		}
		mInputLayout = LowLevelRenderer::createInputLayout(mMesh->vertexLayout,
									materialInstance->material->vertexShader);
	} else {
		materialInstance->isGood = false;
	}
}