#include "Clair/MaterialInstance.h"

using namespace Clair;

MaterialInstance::MaterialInstance(const Material* const material) {
	CLAIR_ASSERT(material, "Material is null");
	mMaterial = material;
	mCBufferData = new MaterialConstBufferData(0, 0, 0);
	MaterialConstBufferData::copy(mCBufferData, material->getConstBufferData());
}

MaterialInstance::~MaterialInstance() {
	delete mCBufferData;
}