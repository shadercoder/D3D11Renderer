#include "Clair/Object.h"
#include "Clair/Material.h"
#include "LowLevelRenderer.h"
#include "Mesh.h"
#include "InputLayout.h"

using namespace Clair;

Object::~Object() {
	for (const auto& it : mMatInstances) {
		delete it.second;
	}
	delete mInputLayout;
}

void Object::setMesh(Mesh* const mesh) {
	mMesh = mesh;
	for (const auto& instance : mMatInstances) {
		recreateInputLayout(instance.second);
	}
}

MaterialInstance* Object::setMaterial(const RenderPass pass,
									  Material* const material) {
	CLAIR_ASSERT(material, "Material is null");
	MaterialInstance* const instance {new MaterialInstance{material}};
	if (mMatInstances.count(pass) != 0) {
		delete mMatInstances[pass];
	}
	mMatInstances[pass] = instance;
	recreateInputLayout(instance);
	return instance;
}

MaterialInstance* Object::getMaterial(const RenderPass pass) {
	if (mMatInstances.count(pass) == 0) {
		return nullptr;
	}
	auto const material = mMatInstances[pass];
	//if (!material->isValid()) return nullptr;
	return material;
}

void Object::recreateInputLayout(MaterialInstance* const materialInstance) {
	bool isCompatible {true};
	if (isCompatible) {
		//materialInstance->isValid = true;
		if (mInputLayout) {
			delete mInputLayout;
		}
		mInputLayout = new InputLayout {
			mMesh->getVertexLayout(),
			materialInstance->getMaterial()->getVertexShader()};
	} else {
		//materialInstance->isValid = false;
	}
}