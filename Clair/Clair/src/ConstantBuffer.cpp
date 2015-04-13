#include "ConstantBuffer.h"
#include <d3d11.h>
#include "LowLevelRenderer.h"

using namespace Clair;

ConstantBuffer::ConstantBuffer(const size_t size) {
	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constBufferDesc.ByteWidth = size;
	constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = 0;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constInitData;
	ZeroMemory(&constInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	const unsigned testInitData[] {0};
	constInitData.pSysMem = testInitData;

	const HRESULT result {
		LowLevelRenderer::getD3dDevice()->CreateBuffer(&constBufferDesc,
													   &constInitData,
													   &mD3dBuffer)};
	if (!FAILED(result)) {
		mIsValid = true;
	}
}

ConstantBuffer::~ConstantBuffer() {
	if (mD3dBuffer) {
		mD3dBuffer->Release();
	}
}
