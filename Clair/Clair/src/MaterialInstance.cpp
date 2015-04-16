#include "Clair/MaterialInstance.h"

using namespace Clair;

MaterialInstance::MaterialInstance(const Material* const material) {
	CLAIR_ASSERT(material, "Material is null");
	mMaterial = material;
	mCBufferData.sizePs = material->getConstBufferData()->sizePs;
	mCBufferData.dataPs = new char[mCBufferData.sizePs];
	memcpy(mCBufferData.dataPs, material->getConstBufferData()->dataPs,
		   material->getConstBufferData()->sizePs);
}

MaterialInstance::~MaterialInstance() {
	//if (mCBufferData.dataVs) delete[] mCBufferData.dataVs;
	//if (mCBufferData.dataGs) delete[] mCBufferData.dataGs;
	//if (mCBufferData.dataPs) delete[] mCBufferData.dataPs;
}