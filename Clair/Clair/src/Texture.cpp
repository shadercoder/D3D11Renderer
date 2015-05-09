#include "Clair/Texture.h"
#include "D3dDevice.h"
#include "Clair/Debug.h"
#include "Clair/ShaderResource.h"
#include "Clair/RenderTarget.h"

using namespace Clair;

#define TEXTURE_FORMAT_CASE(FORMAT, ELEMENT_SIZE) case Format::##FORMAT:\
	texDesc.Format = DXGI_FORMAT_##FORMAT;\
	mD3dFormat = texDesc.Format;\
	elementSize = ELEMENT_SIZE;\
	break;

void Texture::initialize(const Options& options) {
	CLAIR_ASSERT(options.width >= 1 && options.height >= 1,
		"Invalid texture dimensions");
	CLAIR_ASSERT(options.arraySize >= 1 && options.arraySize <= 6,
		"Invalid array size");
	mOptions = options;
	mIsValid = true;
	mNumMips =
		maxPossibleMips(options.width, options.height, options.maxMipLevels);
	auto const d3dDevice = D3dDevice::getD3dDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = static_cast<UINT>(options.width);
	texDesc.Height = static_cast<UINT>(options.height);
	texDesc.MipLevels = mNumMips;
	texDesc.ArraySize = options.arraySize;
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
	if (options.isCubeMap) {
		CLAIR_ASSERT(options.arraySize == 6,
			"Cube map requires texture array size of 6");
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	Byte*** texData = new Byte**[options.arraySize]();
	D3D11_SUBRESOURCE_DATA* subResourceData =
		new D3D11_SUBRESOURCE_DATA[mNumMips * options.arraySize];
	for (size_t i_arr {0}; i_arr < options.arraySize; ++i_arr) {
		texData[i_arr] = new Byte*[mNumMips]();
		int mipWidth = options.width;
		int mipHeight = options.height;
		for (size_t i_mip {0}; i_mip < mNumMips; ++i_mip) {
			const size_t subSize {mipWidth * mipHeight * elementSize};
			texData[i_arr][i_mip] = new Byte[subSize]();
			if (options.initialData && i_mip == 0) {
				memcpy(texData[i_arr][i_mip], options.initialData, subSize);
			}
			const size_t subResIdx = i_mip + i_arr * mNumMips;
			ZeroMemory(&subResourceData[subResIdx],
				sizeof(D3D11_SUBRESOURCE_DATA));
			subResourceData[subResIdx].pSysMem = texData[i_arr][i_mip];
			subResourceData[subResIdx].SysMemPitch =
				sizeof(Byte) * mipWidth * elementSize;
			subResourceData[subResIdx].SysMemSlicePitch = 0;
			mipWidth = max(mipWidth / 2, 1);
			mipHeight = max(mipHeight / 2, 1);
		}
	}
	if (FAILED(d3dDevice->CreateTexture2D(&texDesc, &subResourceData[0],
										  &mD3dTexture))) {
		mIsValid = false;
		return;
	}
	delete[] subResourceData;
	for (size_t a {0}; a < options.arraySize; ++a) {
		for (size_t m {0}; m < mNumMips; ++m) {
			delete[] texData[a][m];
		}
		delete[] texData[a];
	}
	delete[] texData;
	ID3D11ShaderResourceView* newShaderResView {nullptr};
	if (options.type != Type::DEPTH_STENCIL_TARGET) {
		if (FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture,
			nullptr, &newShaderResView))) {
			mIsValid = false;
		}
	} else if (options.isCubeMap) {
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MipLevels =  texDesc.MipLevels;
		viewDesc.TextureCube.MostDetailedMip = 0;
		if (FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture, &viewDesc,
			&newShaderResView))) {
			mIsValid = false;
		}
	} else {
		if (FAILED(d3dDevice->CreateDepthStencilView(mD3dTexture, nullptr,
			&mD3dDepthStencilTargetView))) {
			mIsValid = false;
			return;
		}
	}
	// TODO: TEMP IF
	if (options.type != Type::DEPTH_STENCIL_TARGET)
		mShaderResource = new ShaderResource{newShaderResView};

	if (options.type == Type::RENDER_TARGET) {
		ID3D11RenderTargetView* newRenderTargetView {nullptr};
		if (FAILED(d3dDevice->CreateRenderTargetView(
				mD3dTexture, nullptr, &newRenderTargetView))) {
			mIsValid = false;
		}
		mRenderTarget = new RenderTarget{newRenderTargetView};
		//for (size_t i_arr {0}; i_arr < options.arraySize; ++i_arr) {
		//	for (size_t i_mip {0}; i_mip < mNumMips; ++i_mip) {
		//		D3D11_RENDER_TARGET_VIEW_DESC renderViewDesc;
		//		ZeroMemory(&renderViewDesc,
		//			sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		//		renderViewDesc.Texture2DArray.MipSlice = i_mip;
		//		renderViewDesc.Texture2DArray.FirstArraySlice = i_arr;
		//		renderViewDesc.Texture2DArray.ArraySize = options.arraySize;
		//		renderViewDesc.Format = texDesc.Format;
		//		renderViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		//		ID3D11RenderTargetView* renderTargetView {nullptr};
		//		if (FAILED(d3dDevice->CreateRenderTargetView(
		//				mD3dTexture, &renderViewDesc, &renderTargetView))) {
		//			mIsValid = false;
		//			return;
		//		}
		//		mD3dRenderTargetViews.push_back(renderTargetView);
		//	}
		//}
	}
}

void Texture::clear(const float value) {
	CLAIR_ASSERT(mOptions.type == Type::DEPTH_STENCIL_TARGET,
		"Clear value doesn't match Texture Format")
	if (!mIsValid) {
		return;
	}
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

RenderTarget* Texture::createCustomRenderTarget(
	const size_t arrayStartIndex, const size_t arraySize,
	const size_t mipStartIndex, const size_t numMips,
	const bool isCubeMap) {
	CLAIR_ASSERT(arrayStartIndex + arraySize <= mOptions.arraySize,
		"Incorrect array indices");
	CLAIR_ASSERT(mipStartIndex + numMips <= mNumMips,
		"Incorrect mip map indices");
	CLAIR_ASSERT(mOptions.type != Type::DEPTH_STENCIL_TARGET,
		"DEPTH_STENCIL_TARGETs not supported yet");
	if (isCubeMap) {
		CLAIR_ASSERT(arrayStartIndex == 0 && arraySize == 6,
			"Unable to create a cube map render target"
			"when arraySize is not 6");
	}
	if (mOptions.type == Type::RENDER_TARGET) {
		CLAIR_ASSERT(numMips == 1,
			"Render targets do not support multiple mip map levels")
	}
	CLAIR_DEBUG_LOG_IF(mOptions.isCubeMap && arraySize == 6 && !isCubeMap,
		"Warning: "
		"The parent Texture is a cube map and the render target has an"
		"arraySize of 6, are you sure you didn't want to make this a cube map"
		"as well?");
	auto const d3dDevice = D3dDevice::getD3dDevice();
	ID3D11RenderTargetView* newRenderTargetView {nullptr};
	if (mOptions.type == Type::RENDER_TARGET) {
		D3D11_RENDER_TARGET_VIEW_DESC renderViewDesc;
		ZeroMemory(&renderViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		renderViewDesc.Format = static_cast<DXGI_FORMAT>(mD3dFormat);
		renderViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		renderViewDesc.Texture2DArray.MipSlice = mipStartIndex;
		renderViewDesc.Texture2DArray.FirstArraySlice = arrayStartIndex;
		renderViewDesc.Texture2DArray.ArraySize = arraySize;
		if (FAILED(d3dDevice->CreateRenderTargetView(
				mD3dTexture, &renderViewDesc, &newRenderTargetView))) {
			newRenderTargetView = nullptr;
		}
	}
	RenderTarget* newCustomRenderTarget {new RenderTarget{newRenderTargetView}};
	mCustomRenderTargets.push_back(newCustomRenderTarget);
	return newCustomRenderTarget;
}

ShaderResource* Texture::createCustomShaderResource(
	const size_t arrayStartIndex, const size_t arraySize,
	const size_t mipStartIndex, const size_t numMips,
	const bool isCubeMap) {
	CLAIR_ASSERT(arrayStartIndex + arraySize <= mOptions.arraySize,
		"Incorrect array indices");
	CLAIR_ASSERT(mipStartIndex + numMips <= mNumMips,
		"Incorrect mip map indices");
	CLAIR_ASSERT(mOptions.type != Type::DEPTH_STENCIL_TARGET,
		"DEPTH_STENCIL_TARGETs not supported yet");
	if (isCubeMap) {
		CLAIR_ASSERT(arrayStartIndex == 0 && arraySize == 6,
			"Unable to create a cube map SubTexture when arraySize is not 6");
	}
	CLAIR_DEBUG_LOG_IF(mOptions.isCubeMap && arraySize == 6 && !isCubeMap,
		"Warning: "
		"The parent Texture is a cube map and the SubTexture has an arraySize"
		"of 6, are you sure you didn't want to make this a cube map as well?");
	auto const d3dDevice = D3dDevice::getD3dDevice();
	ID3D11ShaderResourceView* newShaderResView {nullptr};
	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = static_cast<DXGI_FORMAT>(mD3dFormat);
	if (isCubeMap) {
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MipLevels = numMips;
		viewDesc.TextureCube.MostDetailedMip = mipStartIndex;
	} else {
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MipLevels = numMips;
		viewDesc.Texture2DArray.MostDetailedMip = mipStartIndex;
		viewDesc.Texture2DArray.FirstArraySlice = arrayStartIndex;
		viewDesc.Texture2DArray.ArraySize = arraySize;
	}
	if (FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture, &viewDesc,
		&newShaderResView))) {
		newShaderResView = nullptr;
	}
	ShaderResource* newCustomShadResView {new ShaderResource{newShaderResView}};
	mCustomShaderResources.push_back(newCustomShadResView);
	return newCustomShadResView;
}

Texture::~Texture() {
	for (const auto& it : mCustomShaderResources) {
		delete it;
	}
	for (const auto& it : mCustomRenderTargets) {
		delete it;
	}
	destroyD3dObjects();
}

void Texture::destroyD3dObjects() {
	//if (!mIsValid) return;
	if (mD3dDepthStencilTargetView) {
		mD3dDepthStencilTargetView->Release();
	}
	if (mRenderTarget) {
		delete mRenderTarget;
	}
	if (mD3dTexture) {
		mD3dTexture->Release();
	}
	if (mShaderResource) {
		delete mShaderResource;
	}
}

size_t Texture::maxPossibleMips(int width, int height, const size_t max) {
	size_t numLevels = 1;
	while(width > 1 || height > 1) {
		width = max(width / 2, 1);
		height = max(height / 2, 1);
		++numLevels;
		if (max != 0 && numLevels >= max) {
			return max;
		}
	}
	return numLevels;
}

void Texture::getMipMapDimensions(
	const size_t mipLevel, int* outWidth, int* outHeight) {
	CLAIR_ASSERT(mipLevel >= 0 && mipLevel < mNumMips,
		"Incorrect mip map level");
	int width = mOptions.width;
	int height = mOptions.height;
	for (size_t i {0}; i < mipLevel; ++i) {
		width = max(width / 2, 1);
		height = max(height / 2, 1);
	}
	*outWidth = width;
	*outHeight = height;
}
