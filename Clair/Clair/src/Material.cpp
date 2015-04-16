#include "Clair/Material.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <d3d11.h>
#include "Serialization.h"
#include "ConstantBuffer.h"
#include "LowLevelRenderer.h"

using namespace Clair;

Material::Material(const char* data) {
	CLAIR_ASSERT(data, "Material data is null");
	mVertexLayout = Serialization::readVertexLayoutFromBytes(data);
	memcpy(&mCBufferData.sizeVs, data, sizeof(unsigned));
	data += sizeof(unsigned);
	memcpy(&mCBufferData.sizePs, data, sizeof(unsigned));
	data += sizeof(unsigned);
	size_t vsSize {0};
	memcpy(&vsSize, data, sizeof(size_t));
	data += sizeof(size_t);
	const char* const vsData = data;
	data += sizeof(char) * vsSize;
	size_t psSize {0};
	memcpy(&psSize, data, sizeof(size_t));
	data += sizeof(size_t);
	const char* const psData = data;
	mVertexShader = new VertexShader{vsData, vsSize};
	mPixelShader = new PixelShader{psData, psSize};
	mIsValid = mVertexShader->isValid() && mPixelShader->isValid();
	if (mCBufferData.sizePs > 0) {
		mCBufferPs = new ConstantBuffer{mCBufferData.sizePs};
		mCBufferData.dataPs = new char[mCBufferData.sizePs]{};
	}
	CLAIR_LOG_IF(!mIsValid, "Invalid material created");
}

Material::~Material() {
	delete mVertexShader;
	delete mPixelShader;
	if (mCBufferVs) delete mCBufferVs;
	if (mCBufferGs) delete mCBufferGs;
	if (mCBufferPs) delete mCBufferPs;
}
