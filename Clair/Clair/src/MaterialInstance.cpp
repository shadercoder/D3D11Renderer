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

void MaterialInstance::setTexture(unsigned index, const Texture* texture) {
	CLAIR_ASSERT(texture, "Texture shouldn't be null");
	CLAIR_DEBUG_LOG_IF(mTextureMap.count(index) != 0,
					   "Overriding existing texture");
	mTextureMap[index] = texture;
}
