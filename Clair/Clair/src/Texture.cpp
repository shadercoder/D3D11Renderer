#include "Clair/Texture.h"
#include "D3dDevice.h"
#include "Clair/Debug.h"

using namespace Clair;

#define TEXTURE_FORMAT_CASE(FORMAT, ELEMENT_SIZE) case Format::##FORMAT:\
	texDesc.Format = DXGI_FORMAT_##FORMAT;\
	elementSize = ELEMENT_SIZE;\
	break;

void Texture::initialize(const Options& options) {
	CLAIR_ASSERT(options.width >= 1 && options.height >= 1,
		"Invalid texture dimensions");
	CLAIR_ASSERT(options.arraySize >= 1 && options.arraySize <= 6,
		"Invalid array size");
	mOptions = options;
	auto const d3dDevice = D3dDevice::getD3dDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = static_cast<UINT>(options.width);
	texDesc.Height = static_cast<UINT>(options.height);
	texDesc.MipLevels = static_cast<UINT>(options.mipLevels);
	texDesc.ArraySize = static_cast<UINT>(options.arraySize);
	size_t elementSize {0};
	switch (options.format) {
	TEXTURE_FORMAT_CASE(R8G8B8A8_UNORM, 4);
	TEXTURE_FORMAT_CASE(R32G32B32A32_FLOAT, 16);
	TEXTURE_FORMAT_CASE(D24_UNORM_S8_UINT, 4);
	}
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	if (options.type == Type::RENDER_TARGET) {
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
	} else if (options.type == Type::DEPTH_STENCIL_TARGET) {
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	} else {
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	texDesc.CPUAccessFlags = 0;
	if (options.type == Type::CUBE_MAP_DEFAULT) {
		CLAIR_ASSERT(options.arraySize == 6,
			"Cube map requires texture array size of 6");
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	Byte*** texData = new Byte**[options.arraySize]();
	D3D11_SUBRESOURCE_DATA* subResourceData =
		new D3D11_SUBRESOURCE_DATA[options.mipLevels * options.arraySize];
	for (size_t i_arr {0}; i_arr < options.arraySize; ++i_arr) {
		texData[i_arr] = new Byte*[options.mipLevels]();
		int mipWidth = options.width;
		int mipHeight = options.height;
		for (size_t i_mip {0}; i_mip < options.mipLevels; ++i_mip) {
			if (i_mip > 0) {
				mipWidth /= 2;
				mipHeight /= 2;
			}
			const size_t subResIdx = i_mip + i_arr * options.mipLevels;
			ZeroMemory(&subResourceData[subResIdx],
				sizeof(D3D11_SUBRESOURCE_DATA));
			texData[i_arr][i_mip] =
				new Byte[mipWidth * mipHeight * elementSize]();
			subResourceData[subResIdx].pSysMem = texData[i_arr][i_mip];
			subResourceData[subResIdx].SysMemPitch =
				sizeof(Byte) * mipWidth * elementSize;
			subResourceData[subResIdx].SysMemSlicePitch = 0;
			if (options.initialData && i_mip == 0) {
				for (int y {0}; y < mipHeight; ++y) {
					for (int x {0}; x < mipWidth; ++x) {
						const int idx = (x + y * mipWidth) * 4;
						texData[i_arr][i_mip][idx + 0] =
							options.initialData[idx + 0];
						texData[i_arr][i_mip][idx + 1] =
							options.initialData[idx + 1];
						texData[i_arr][i_mip][idx + 2] =
							options.initialData[idx + 2];
						texData[i_arr][i_mip][idx + 3] = 255;
					}
				}
			}
		}
	}
	mIsValid = !FAILED(d3dDevice->CreateTexture2D(&texDesc, &subResourceData[0],
												  &mD3dTexture));
	delete[] subResourceData;
	for (size_t a {0}; a < options.arraySize; ++a) {
		for (size_t m {0}; m < options.mipLevels; ++m) {
			delete[] texData[a][m];
		}
		delete[] texData[a];
	}
	delete[] texData;
	if (options.type != Type::DEPTH_STENCIL_TARGET) {
		if (FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture,
			nullptr, &mD3dShaderResView))) {
			mIsValid = false;
			return;
		}
	} else if (options.type == Type::CUBE_MAP_DEFAULT) {
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MipLevels =  texDesc.MipLevels;
		viewDesc.TextureCube.MostDetailedMip = 0;
		if (FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture, &viewDesc,
			&mD3dShaderResView))) {
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

	if (options.type == Type::RENDER_TARGET) {
		if (FAILED(d3dDevice->CreateRenderTargetView(mD3dTexture, nullptr,
			&mD3dRenderTargetView))) {
			mIsValid = false;
			return;
		}
	}
}

void Texture::clear(const Float4& value) {
	CLAIR_ASSERT(mOptions.type == Type::RENDER_TARGET,
		"Clear value doesn't match Texture Format")
	auto d3dContext = D3dDevice::getD3dDeviceContext();
	d3dContext->ClearRenderTargetView(mD3dRenderTargetView, &value[0][0]);
}

void Texture::clear(const float value) {
	CLAIR_ASSERT(mOptions.type == Type::DEPTH_STENCIL_TARGET,
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
	Options newOptions (mOptions);
	newOptions.width = width;
	newOptions.height = height;
	initialize(newOptions);
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