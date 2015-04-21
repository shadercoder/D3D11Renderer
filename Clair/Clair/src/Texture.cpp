#include "Clair/Texture.h"
#include "LowLevelRenderer.h"
#include <d3d11.h>

using namespace Clair;

Texture::~Texture() {
	if (mD3dTexture) {
		mD3dTexture->Release();
	}
	if (mD3dShaderResView) {
		mD3dShaderResView->Release();
	}
}

void Texture::initialize(const int width, const int height,
						 const Byte* data) {
	CLAIR_ASSERT(width > 0 && height > 0 && data, "Invalid texture parameters");
	auto const d3dDevice = LowLevelRenderer::getD3dDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = static_cast<UINT>(width);
	texDesc.Height = static_cast<UINT>(height);
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

	Byte* texData = new Byte[width * height * 4]();
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const int idx = (x + y * width) * 4;
			texData[idx + 0] = data[idx + 0];
			texData[idx + 1] = data[idx + 1];
			texData[idx + 2] = data[idx + 2];
			texData[idx + 3] = 127;
		}
	}
	D3D11_SUBRESOURCE_DATA texInitData;
	ZeroMemory(&texInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitData.pSysMem = texData;
	texInitData.SysMemPitch = sizeof(Byte) * width * 4;

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