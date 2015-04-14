#include "VertexShader.h"
#include <d3d11.h>
#include "LowLevelRenderer.h"

using namespace Clair;
VertexShader::VertexShader(const char* const byteCode,
						   const unsigned byteCodeSize)
	: mByteCode		{byteCode}
	, mByteCodeSize	{byteCodeSize} {
	ID3D11Device* const d3dDevice = LowLevelRenderer::getD3dDevice();
	const HRESULT result {
		d3dDevice->CreateVertexShader(byteCode, byteCodeSize,
									  nullptr, &mD3dShader)
	};
	if (!FAILED(result)) {
		mIsValid = true;
	}
}

VertexShader::~VertexShader() {
	if (mD3dShader) {
		mD3dShader->Release();
	}
}
