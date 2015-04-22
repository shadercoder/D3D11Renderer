#include "PixelShader.h"
#include "D3dDevice.h"

using namespace Clair;

PixelShader::PixelShader(const Byte* const byteCode,
						 const unsigned byteCodeSize) {
	ID3D11Device* const d3dDevice = D3dDevice::getD3dDevice();
	const HRESULT result {
		d3dDevice->CreatePixelShader(byteCode, byteCodeSize,
									 nullptr, &mD3dShader)
	};
	if (!FAILED(result)) {
		mIsValid = true;
	}
}

PixelShader::~PixelShader() {
	if (mD3dShader) {
		mD3dShader->Release();
	}
}

