#include "VertexBuffer.h"
#include <d3d11.h>

using namespace Clair;

VertexBuffer::VertexBuffer(ID3D11Device* const d3dDevice,
						   const char* const bufferData,
						   const unsigned bufferSize) {
	D3D11_BUFFER_DESC vertexBufferDesc {};
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.ByteWidth = bufferSize;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResData {};
	ZeroMemory(&subResData, sizeof(D3D11_SUBRESOURCE_DATA));
	subResData.pSysMem = bufferData;

	const HRESULT result {
		d3dDevice->CreateBuffer(&vertexBufferDesc, &subResData, &mD3dBuffer)
	};
	if (!FAILED(result)) {
		mIsValid = true;
	}
}

VertexBuffer::~VertexBuffer() {
	if (mD3dBuffer) {
		mD3dBuffer->Release();
	}
}

