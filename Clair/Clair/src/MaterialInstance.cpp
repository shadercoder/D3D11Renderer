#include "Clair/MaterialInstance.h"

using namespace Clair;

MaterialInstance::MaterialInstance(const Material* const material) {
	CLAIR_ASSERT(material, "Material is null");
	mMaterial = material;
	memcpy(&mCBufferData,
		   material->getConstBufferData(),
		   sizeof(MaterialConstBufferData));
}
