#include "Clair/DepthStencilTarget.h"
#include "D3dDevice.h"
#include "Clair/Debug.h"

using namespace Clair;

void DepthStencilTarget::initialize(Texture* const texture) {
	CLAIR_ASSERT(texture, "Texture should not be null");
	CLAIR_ASSERT(texture->isValid(), "Texture is invalid");
	CLAIR_ASSERT(texture->getType() == Texture::Type::DEPTH_STENCIL_TARGET,
		"Texture does not support RTT");
	CLAIR_DEBUG_LOG_IF(mTexture, "Overwriting existing texture");
	mTexture = texture;
	HRESULT result {};
	auto d3dDevice = D3dDevice::getD3dDevice();
	result = d3dDevice->CreateDepthStencilView(mTexture->mD3dTexture, nullptr,
											   &mD3dDepthStencilTargetView);
	mIsValid = !FAILED(result);
}

void DepthStencilTarget::clearDepth(float value) {
	auto d3dDeviceContext = D3dDevice::getD3dDeviceContext();
	d3dDeviceContext->ClearDepthStencilView(
										mD3dDepthStencilTargetView,
										D3D11_CLEAR_DEPTH,
										value, 0);
	d3dDeviceContext->Release();
}

DepthStencilTarget::~DepthStencilTarget() {
	if (mD3dDepthStencilTargetView) {
		mD3dDepthStencilTargetView->Release();
	}
}