#include "clair/object.h"
#include "material.h"

using namespace Clair;

void Object::setMaterial(const RenderPass pass,
							  Material* const subMaterial) {
	if (mSubMaterials.count(pass) == 0) {
		mSubMaterials[pass] = subMaterial;
	}
}

Material* Object::getMaterial(const RenderPass pass) {
	if (mSubMaterials.count(pass) == 0) {
		return nullptr;
	}
	return mSubMaterials[pass];
}