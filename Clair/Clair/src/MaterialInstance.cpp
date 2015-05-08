#include "Clair/MaterialInstance.h"

using namespace Clair;

void MaterialInstance::initialize(const Material* const material) {
	CLAIR_ASSERT(material, "Material is null");
	mMaterial = material;
	mCBufferData = new MaterialConstBufferData(0, 0, 0);
	MaterialConstBufferData::copy(mCBufferData, material->getConstBufferData());
	mIsValid = true;
}

MaterialInstance::~MaterialInstance() {
	delete mCBufferData;
}

void MaterialInstance::setShaderResource(
	const unsigned index, const ShaderResource* const resource) {
	CLAIR_ASSERT(resource, "Texture shouldn't be null");
	CLAIR_DEBUG_LOG_IF(mShaderResourceMap.count(index) != 0,
					   "Overriding existing shader resource");
	mShaderResourceMap[index] = resource;
}