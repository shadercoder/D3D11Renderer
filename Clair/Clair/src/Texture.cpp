#include "Clair/Texture.h"
#include "D3dDevice.h"
#include "Clair/Debug.h"

using namespace Clair;

#define TEXTURE_FORMAT_CASE(a) case Format::##a:\
	texDesc.Format = DXGI_FORMAT_##a;\
	break;

void Texture::initialize(const int width, const int height,
						 const Byte* data, const Format format,
						 const Type type) {
	CLAIR_ASSERT(width > 0 && height > 0, "Invalid texture dimensions");
	mFormat = format;
	mType = type;
	auto const d3dDevice = D3dDevice::getD3dDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = static_cast<UINT>(width);
	texDesc.Height = static_cast<UINT>(height);
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	switch (format) {
	TEXTURE_FORMAT_CASE(R8G8B8A8_UNORM);
	TEXTURE_FORMAT_CASE(R32G32B32A32_FLOAT);
	TEXTURE_FORMAT_CASE(D24_UNORM_S8_UINT);
	}
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	if (type == Type::RENDER_TARGET) {
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
	} else if (type == Type::DEPTH_STENCIL_TARGET) {
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
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
		if (FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture,
			nullptr, &mD3dShaderResView))) {
			mIsValid = false;
			return;
		}
	} else {
		if (FAILED(d3dDevice->CreateDepthStencilView(mD3dTexture, nullptr,
			&mD3dDepthStencilTargetView))) {
			mIsValid = false;
			return;
		}
	}

	if (type == Type::RENDER_TARGET) {
		if (FAILED(d3dDevice->CreateRenderTargetView(mD3dTexture, nullptr,
			&mD3dRenderTargetView))) {
			mIsValid = false;
			return;
		}
	}
}

void Texture::clear(const Float4& value) {
	CLAIR_ASSERT(mType == Type::RENDER_TARGET,
		"Clear value doesn't match Texture Format")
	auto d3dContext = D3dDevice::getD3dDeviceContext();
	d3dContext->ClearRenderTargetView(mD3dRenderTargetView, &value[0][0]);
}

void Texture::clear(const float value) {
	CLAIR_ASSERT(mType == Type::DEPTH_STENCIL_TARGET,
		"Clear value doesn't match Texture Format")
	auto d3dDeviceContext = D3dDevice::getD3dDeviceContext();
	d3dDeviceContext->ClearDepthStencilView(
		mD3dDepthStencilTargetView,
		D3D11_CLEAR_DEPTH,
		value, 0);
}


void Texture::resize(const int width, const int height) {
	CLAIR_ASSERT(width > 0 && height > 0, "Invalid texture dimensions");
	destroyD3dObjects();
	initialize(width, height, nullptr, mFormat, mType);
}

Texture::~Texture() {
	destroyD3dObjects();
}

void Texture::destroyD3dObjects() {
	//if (!mIsValid) return;
	if (mD3dDepthStencilTargetView) {
		mD3dDepthStencilTargetView->Release();
	}
	if (mD3dRenderTargetView) {
		mD3dRenderTargetView->Release();
	}
	if (mD3dTexture) {
		mD3dTexture->Release();
	}
	if (mD3dShaderResView) {
		mD3dShaderResView->Release();
	}
}