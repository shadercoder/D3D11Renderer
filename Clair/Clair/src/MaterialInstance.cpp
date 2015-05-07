#include "Clair/MaterialInstance.h"
#include "SubTexture.h"

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

void MaterialInstance::setTexture(
	const unsigned index, const Texture* const texture) {
	CLAIR_ASSERT(texture, "Texture shouldn't be null");
	CLAIR_DEBUG_LOG_IF(mTextureMap.count(index) != 0,
					   "Overriding existing texture");
	mTextureMap[index] = texture->getD3DShaderResourceView();
}

void MaterialInstance::setTexture(
	const unsigned index, const SubTexture* const subTexture) {
	CLAIR_ASSERT(subTexture, "Texture shouldn't be null");
	CLAIR_DEBUG_LOG_IF(mTextureMap.count(index) != 0,
					   "Overriding existing texture");
	mTextureMap[index] = subTexture->getD3DShaderResourceView();
}