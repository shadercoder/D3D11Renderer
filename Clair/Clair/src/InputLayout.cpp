#include "InputLayout.h"
#include <d3d11.h>
#include "VertexShader.h"
#include "LowLevelRenderer.h"
#include "Clair/Debug.h"

using namespace Clair;

InputLayout::InputLayout(const VertexLayout& vertexLayout,
						 const VertexShader* const vertexShader) {
	CLAIR_ASSERT(vertexShader, "Vertex shader is null");
	std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc {};
	unsigned offset {0};
	for (const auto& it : vertexLayout) {
		auto format = DXGI_FORMAT_R32_FLOAT;
		offset = mStride;
		switch (it.format) {
		case VertexAttribute::Format::FLOAT1: mStride += sizeof(float) * 1;
			format = DXGI_FORMAT_R32_FLOAT; break;
		case VertexAttribute::Format::FLOAT2: mStride += sizeof(float) * 2;
			format = DXGI_FORMAT_R32G32_FLOAT; break;
		case VertexAttribute::Format::FLOAT3: mStride += sizeof(float) * 3;
			format = DXGI_FORMAT_R32G32B32_FLOAT; break;
		case VertexAttribute::Format::FLOAT4: mStride += sizeof(float) * 4;
			format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		}
		layoutDesc.push_back({it.name.c_str(), 0, format, 0, offset,
							  D3D11_INPUT_PER_VERTEX_DATA, 0});
	}
	ID3D11Device* const d3dDevice = LowLevelRenderer::getD3dDevice();
	const HRESULT result {
		d3dDevice->CreateInputLayout(layoutDesc.data(), layoutDesc.size(),
										  vertexShader->getByteCode(),
										  vertexShader->getByteCodeSize(),
										  &mD3dInputLayout)
	};
	if (!FAILED(result)) {
		mIsValid = true;
	}
}

InputLayout::~InputLayout() {
	if (mD3dInputLayout) {
		mD3dInputLayout->Release();
	}
}
