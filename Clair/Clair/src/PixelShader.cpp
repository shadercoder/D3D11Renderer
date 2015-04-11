#include "PixelShader.h"
#include <d3d11.h>

using namespace Clair;

PixelShader::PixelShader(ID3D11Device* const d3dDevice,
						 const char* const byteCode,
						 const unsigned byteCodeSize) {
	HRESULT result {d3dDevice->CreatePixelShader(byteCode, byteCodeSize, nullptr,
					&mD3dShader)};
	if (!FAILED(result)) {
		mIsValid = true;
	}
}

PixelShader::~PixelShader() {
	if (mD3dShader) {
		mD3dShader->Release();
	}
}

