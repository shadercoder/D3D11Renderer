#include "Clair/Texture.h"
#include "LowLevelRenderer.h"
#include <d3d11.h>

using namespace Clair;

Texture::Texture(const Byte* const data) {
	auto const d3dDevice = LowLevelRenderer::getD3dDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	const int size = 512;
	texDesc.Width = size;
	texDesc.Height = size;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	//texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	Byte* texData = new Byte[size * size * 4]();
	for (unsigned y = 0; y < size; ++y) {
		for (unsigned x = 0; x < size; ++x) {
			const unsigned idx = (x + y * size) * 4;
			texData[idx + 0] = data[idx + 0];
			texData[idx + 1] = data[idx + 1];
			texData[idx + 2] = data[idx + 2];
			texData[idx + 3] = 127;
		}
	}
	D3D11_SUBRESOURCE_DATA texInitData;
	ZeroMemory(&texInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitData.pSysMem = texData;
	texInitData.SysMemPitch = sizeof(Byte) * 512 * 4;

	if (!FAILED(d3dDevice->CreateTexture2D(&texDesc, &texInitData,
										   &mD3dTexture))) {
		mIsValid = true;
	}
	delete[] texData;
	if (!FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture, nullptr,
												   &mD3dShaderResView))) {
		mIsValid = true;
	}
}

Texture::~Texture() {
	if (mD3dTexture) {
		mD3dTexture->Release();
	}
	if (mD3dShaderResView) {
		mD3dShaderResView->Release();
	}
}