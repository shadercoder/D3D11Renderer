#include "VertexShader.h"
#include "D3dDevice.h"

using namespace Clair;

VertexShader::VertexShader(const Byte* const byteCode,
						   const unsigned byteCodeSize)
	: mByteCodeSize	{byteCodeSize} {
	mByteCode = new Byte[byteCodeSize];
	memcpy(mByteCode, byteCode, byteCodeSize);
	ID3D11Device* const d3dDevice = D3dDevice::getD3dDevice();
	const HRESULT result {
		d3dDevice->CreateVertexShader(byteCode, byteCodeSize,
									  nullptr, &mD3dShader)
	};
	if (!FAILED(result)) {
		mIsValid = true;
	}
}

VertexShader::~VertexShader() {
	delete[] mByteCode;
	if (mD3dShader) {
		mD3dShader->Release();
	}
}
