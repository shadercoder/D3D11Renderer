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
	unsigned dataSizeVs {0};
	unsigned dataSizeGs {0};
	unsigned dataSizePs {0};
	memcpy(&dataSizeVs, data, sizeof(unsigned));
	data += sizeof(unsigned);
	//memcpy(&dataSizeGs, data, sizeof(unsigned));
	//data += sizeof(unsigned);
	memcpy(&dataSizePs, data, sizeof(unsigned));
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
	if (dataSizePs > 0) {
		mCBufferPs = new ConstantBuffer{dataSizePs};
	}
	mCBufferData = new MaterialConstBufferData{
		dataSizeVs, dataSizeGs, dataSizePs};
	CLAIR_LOG_IF(!mIsValid, "Invalid material created");
}

Material::~Material() {
	delete mVertexShader;
	delete mPixelShader;
	delete mCBufferData;
	if (mCBufferVs) delete mCBufferVs;
	if (mCBufferGs) delete mCBufferGs;
	if (mCBufferPs) delete mCBufferPs;
}
