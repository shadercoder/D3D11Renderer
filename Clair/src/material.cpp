#include "clair/material.h"

using namespace Clair;

void Material::setSubMaterial(const RenderPass pass,
							  SubMaterial* const subMaterial) {
	if (mSubMaterials.count(pass) == 0) {
		mSubMaterials[pass] = subMaterial;
	}
}

SubMaterial* Material::getSubMaterial(const RenderPass pass) {
	if (mSubMaterials.count(pass) == 0) {
		return nullptr;
	}
	return mSubMaterials[pass];
}
