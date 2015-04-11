#include "IndexBuffer.h"
#include <d3d11.h>

using namespace Clair;

IndexBuffer::IndexBuffer(ID3D11Device* const d3dDevice,
						 const unsigned* const bufferData,
						 const unsigned bufferSize) {
	D3D11_BUFFER_DESC indexBufferDesc {};
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.ByteWidth = bufferSize;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexInitData {};
	ZeroMemory(&indexInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexInitData.pSysMem = bufferData;

	const HRESULT result {
		d3dDevice->CreateBuffer(&indexBufferDesc, &indexInitData,
								&mD3dBuffer)
	};
	if (!FAILED(result)) {
		mIsValid = true;
	}
}

IndexBuffer::~IndexBuffer() {
	if (mD3dBuffer) {
		mD3dBuffer->Release();
	}
}
