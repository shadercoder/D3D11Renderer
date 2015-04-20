#include "Clair/MaterialConstBufferData.h"
#include <memory>

using namespace Clair;

MaterialConstBufferData::MaterialConstBufferData(
	const unsigned sizeVs, const unsigned sizeGs, const unsigned sizePs) {
	create(sizeVs, sizeGs, sizePs);
}

MaterialConstBufferData::~MaterialConstBufferData() {
	destroy();
}

void MaterialConstBufferData::create(
	const unsigned sizeVs, const unsigned sizeGs, const unsigned sizePs) {
	mSizeVs = sizeVs;	
	mSizeGs = sizeGs;	
	mSizePs = sizePs;	
	if (sizeVs != 0) mDataVs = new Byte[sizeVs];
	if (sizeGs != 0) mDataGs = new Byte[sizeGs];
	if (sizePs != 0) mDataPs = new Byte[sizePs];
}

void MaterialConstBufferData::destroy() {
	if (mDataVs) delete[] mDataVs;
	if (mDataGs) delete[] mDataGs;
	if (mDataPs) delete[] mDataPs;
}

void MaterialConstBufferData::copy(
	MaterialConstBufferData* const to,
	const MaterialConstBufferData* const from) {
	to->destroy();
	to->create(from->getSizeVs(), from->getSizeGs(), from->getSizePs());
	if (from->getSizeVs() != 0) {
		memcpy(to->mDataVs, from->getDataVs(), from->getSizeVs());
	}
	if (from->getSizeGs() != 0) {
		memcpy(to->mDataGs, from->getDataGs(), from->getSizeGs());
	}
	if (from->getSizePs() != 0) {
		memcpy(to->mDataPs, from->getDataPs(), from->getSizePs());
	}
}