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

Texture::Texture() {
	mShaderResource = new ShaderResource{};
	mRenderTarget = new RenderTarget{};
}

Texture::~Texture() {
	delete mRenderTarget;
	delete mShaderResource;
	for (const auto& it : mSubShaderResources) {
		delete it;
	}
	for (const auto& it : mSubRenderTargets) {
		delete it;
	}
	destroyD3dObjects();
}


void Texture::initialize(const Options& options) {
	CLAIR_ASSERT(options.width >= 1 && options.height >= 1,
		"Invalid texture dimensions");
	CLAIR_ASSERT(options.arraySize >= 1 && options.arraySize <= 6,
		"Invalid array size");
	if (options.type == Type::DEPTH_STENCIL_TARGET) {
		CLAIR_ASSERT(options.format == Format::D24_UNORM_S8_UINT,
			"Depth/stencil targets only support the D24_UNORM_S8_UINT type"
			"at the moment");
		CLAIR_ASSERT(!options.isCubeMap, "No cube map depth/stencil supported");
	}
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
	TEXTURE_FORMAT_CASE(R16G16B16A16_FLOAT, 16);
	TEXTURE_FORMAT_CASE(D24_UNORM_S8_UINT, 4);
	TEXTURE_FORMAT_CASE(R16G16_FLOAT, 16);
	}
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (options.type == Type::RENDER_TARGET) {
		texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	} else if (options.type == Type::DEPTH_STENCIL_TARGET) {
		texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
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
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		if (FAILED(d3dDevice->CreateDepthStencilView(mD3dTexture, &dsvDesc,
			&mD3dDepthStencilTargetView))) {
			mIsValid = false;
			return;
		}
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		if (FAILED(d3dDevice->CreateShaderResourceView(
			mD3dTexture, &srvDesc, &newShaderResView))) {
			mIsValid = false;
			return;
		}
	}
	SubTextureOptions o;
	o.arrayStartIndex = 0;
	o.arraySize = options.arraySize;
	o.mipStartIndex = 0;
	o.numMips = mNumMips;
	mShaderResource->terminate();
	mShaderResource->initialize(newShaderResView, o);

	if (options.type == Type::RENDER_TARGET) {
		ID3D11RenderTargetView* newRenderTargetView {nullptr};
		if (FAILED(d3dDevice->CreateRenderTargetView(
				mD3dTexture, nullptr, &newRenderTargetView))) {
			mIsValid = false;
		}

		SubTextureOptions o;
		o.arrayStartIndex = 0;
		o.arraySize = options.arraySize;
		o.mipStartIndex = 0;
		o.numMips = mNumMips;
		mRenderTarget->terminate();
		mRenderTarget->initialize(newRenderTargetView, o);
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
	// All shader resources/render targets have to be rebuilt as well
	for (const auto& it : mSubShaderResources) {
		it->terminate();
		initializeSubShaderRes(it, it->mOptions);
	}
	for (const auto& it : mSubRenderTargets) {
		it->terminate();
		initializeSubRenderTarget(it, it->mOptions);
	}
}

RenderTarget* Texture::createSubRenderTarget(const SubTextureOptions& s) {
	RenderTarget* newCustomRenderTarget {new RenderTarget{}};
	initializeSubRenderTarget(newCustomRenderTarget, s);
	mSubRenderTargets.push_back(newCustomRenderTarget);
	return newCustomRenderTarget;
}

ShaderResource* Texture::createSubShaderResource(const SubTextureOptions& s) {
	ShaderResource* newCustomShadResView {new ShaderResource{}};
	initializeSubShaderRes(newCustomShadResView, s);
	mSubShaderResources.push_back(newCustomShadResView);
	return newCustomShadResView;
}

void Texture::destroyD3dObjects() {
	//if (!mIsValid) return;
	if (mD3dDepthStencilTargetView) {
		mD3dDepthStencilTargetView->Release();
	}
	if (mD3dTexture) {
		mD3dTexture->Release();
	}
}

void Texture::initializeSubShaderRes(
	ShaderResource* outResource,
	const SubTextureOptions& s) {
	CLAIR_ASSERT(outResource, "Resource cannot be null here");
	CLAIR_ASSERT(s.arrayStartIndex + s.arraySize <= mOptions.arraySize,
		"Incorrect array indices");
	CLAIR_ASSERT(s.mipStartIndex + s.numMips <= mNumMips,
		"Incorrect mip map indices");
	CLAIR_ASSERT(mOptions.type != Type::DEPTH_STENCIL_TARGET,
		"DEPTH_STENCIL_TARGETs not supported yet");
	if (s.isCubeMap) {
		CLAIR_ASSERT(s.arrayStartIndex == 0 && s.arraySize == 6,
			"Unable to create a cube map SubTexture when arraySize is not 6");
	}
	CLAIR_DEBUG_LOG_IF(mOptions.isCubeMap && s.arraySize == 6 && !s.isCubeMap,
		"Warning: "
		"The parent Texture is a cube map and the SubTexture has an arraySize"
		"of 6, are you sure you didn't want to make this a cube map as well?");
	auto const d3dDevice = D3dDevice::getD3dDevice();
	ID3D11ShaderResourceView* newShaderResView {nullptr};
	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = static_cast<DXGI_FORMAT>(mD3dFormat);
	if (s.isCubeMap) {
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MipLevels = s.numMips;
		viewDesc.TextureCube.MostDetailedMip = s.mipStartIndex;
	} else {
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MipLevels = s.numMips;
		viewDesc.Texture2DArray.MostDetailedMip = s.mipStartIndex;
		viewDesc.Texture2DArray.FirstArraySlice = s.arrayStartIndex;
		viewDesc.Texture2DArray.ArraySize = s.arraySize;
	}
	if (FAILED(d3dDevice->CreateShaderResourceView(mD3dTexture, &viewDesc,
		&newShaderResView))) {
		newShaderResView = nullptr;
	}
	outResource->initialize(newShaderResView, s);
}

void Texture::initializeSubRenderTarget(
	RenderTarget* outTarget,
	const SubTextureOptions& s) {
	CLAIR_ASSERT(outTarget, "Render target cannot be null here");
	CLAIR_ASSERT(s.arrayStartIndex + s.arraySize <= mOptions.arraySize,
		"Incorrect array indices");
	CLAIR_ASSERT(s.mipStartIndex + s.numMips <= mNumMips,
		"Incorrect mip map indices");
	CLAIR_ASSERT(mOptions.type != Type::DEPTH_STENCIL_TARGET,
		"DEPTH_STENCIL_TARGETs not supported yet");
	if (s.isCubeMap) {
		CLAIR_ASSERT(s.arrayStartIndex == 0 && s.arraySize == 6,
			"Unable to create a cube map render target"
			"when arraySize is not 6");
	}
	if (mOptions.type == Type::RENDER_TARGET) {
		CLAIR_ASSERT(s.numMips == 1,
			"Render targets do not support multiple mip map levels")
	}
	CLAIR_DEBUG_LOG_IF(mOptions.isCubeMap && s.arraySize == 6 && !s.isCubeMap,
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
		renderViewDesc.Texture2DArray.MipSlice = s.mipStartIndex;
		renderViewDesc.Texture2DArray.FirstArraySlice = s.arrayStartIndex;
		renderViewDesc.Texture2DArray.ArraySize = s.arraySize;
		if (FAILED(d3dDevice->CreateRenderTargetView(
				mD3dTexture, &renderViewDesc, &newRenderTargetView))) {
			newRenderTargetView = nullptr;
		}
	}
	outTarget->initialize(newRenderTargetView, s);
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
