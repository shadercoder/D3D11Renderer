#include "Clair/RenderTarget.h"
#include "D3dDevice.h"
#include "Clair/Debug.h"

using namespace Clair;

void RenderTarget::initialize(Texture* const texture) {
	CLAIR_ASSERT(texture, "Texture should not be null");
	CLAIR_ASSERT(texture->isValid(), "Texture is invalid");
	CLAIR_ASSERT(texture->getType() == Texture::Type::RENDER_TARGET,
		"Texture does not support RTT");
	CLAIR_DEBUG_LOG_IF(mTexture, "Overwriting existing texture");
	mTexture = texture;
	HRESULT result {};
	auto d3dDevice = D3dDevice::getD3dDevice();
	result = d3dDevice->CreateRenderTargetView(mTexture->mD3dTexture, nullptr,
											   &mD3dRenderTargetView);
	mIsValid = !FAILED(result);
}

RenderTarget::~RenderTarget() {
	if (!mIsValid) return;
	if (mD3dRenderTargetView) {
		mD3dRenderTargetView->Release();
	}
}

void RenderTarget::clear(const Float4& color) {
	//ID3D11DeviceContext* d3dContext {nullptr};
	//D3dDevice::getD3dDevice()->GetImmediateContext(&d3dContext);
	//d3dContext->Release();
	auto d3dContext = D3dDevice::getD3dDeviceContext();
	d3dContext->ClearRenderTargetView(mD3dRenderTargetView, &color[0][0]);
	//d3dContext->Release();
}