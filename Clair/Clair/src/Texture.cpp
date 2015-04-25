#include "Clair/Texture.h"
#include "D3dDevice.h"
#include "Clair/Debug.h"

using namespace Clair;

Texture::~Texture() {
	if (!mIsValid) return;
	if (mD3dTexture) {
		mD3dTexture->Release();
	}
	if (mD3dShaderResView) {
		mD3dShaderResView->Release();
	}
}

void Texture::initialize(const int width, const int height,
						 const Byte* data, const Type type) {
	//CLAIR_ASSERT(data, "Texture data should not be null");
	CLAIR_ASSERT(width > 0 && height > 0, "Invalid texture dimensions");
	mType = type;
	auto const d3dDevice = D3dDevice::getD3dDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = static_cast<UINT>(width);
	texDesc.Height = static_cast<UINT>(height);
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	if (type == Type::RENDER_TARGET) {
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
	} else if (type == Type::DEPTH_STENCIL_TARGET) {
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	} else {
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	Byte* texData = new Byte[width * height * 4 * 4]();
	if (data) {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				const int idx = (x + y * width) * 4;
				texData[idx + 0] = data[idx + 0];
				texData[idx + 1] = data[idx + 1];
				texData[idx + 2] = data[idx + 2];
				texData[idx + 3] = 127;
			}
		}
	}
	D3D11_SUBRESOURCE_DATA texInitData;
	ZeroMemory(&texInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitData.pSysMem = texData;
	texInitData.SysMemPitch = sizeof(Byte) * width * 4 * 4;

	mIsValid = !FAILED(d3dDevice->CreateTexture2D(&texDesc, &texInitData,
												  &mD3dTexture));
	delete[] texData;
	if (type != Type::DEPTH_STENCIL_TARGET) {
		mIsValid = !FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture,
			nullptr, &mD3dShaderResView));
	}
}
