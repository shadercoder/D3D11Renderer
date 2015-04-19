#include "Clair/Texture.h"
#include "LowLevelRenderer.h"
#include <d3d11.h>

using namespace Clair;

Texture::Texture(const char* const data) {
	auto const d3dDevice = LowLevelRenderer::getD3dDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = 512;
	texDesc.Height = 512;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	//texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	//float* texData = new float[256 * 256 * 4]();
	//for (unsigned y = 0; y < 256; ++y) {
	//	for (unsigned x = 0; x < 256; ++x) {
	//		texData[(x + y * 256) * 4 + 0] = static_cast<float>(x) / 255.0f;
	//		texData[(x + y * 256) * 4 + 1] = static_cast<float>(y) / 255.0f;
	//		texData[(x + y * 256) * 4 + 2] = static_cast<float>(rand()) /
	//										 static_cast<float>(RAND_MAX);
	//		texData[(x + y * 256) * 4 + 3] = 1.0f;
	//	}
	//}

	D3D11_SUBRESOURCE_DATA texInitData;
	ZeroMemory(&texInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitData.pSysMem = data;
	texInitData.SysMemPitch = sizeof(char) * 512 * 3;

	if (!FAILED(d3dDevice->CreateTexture2D(&texDesc, &texInitData,
										   &mD3dTexture))) {
		mIsValid = true;
	}
	//delete[] texData;
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