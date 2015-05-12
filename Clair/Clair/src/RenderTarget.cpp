#include "Clair/RenderTarget.h"
#include "D3dDevice.h"

using namespace Clair;

RenderTarget::RenderTarget() {
}

RenderTarget::~RenderTarget() {
	terminate();
}

void RenderTarget::initialize(
	ID3D11RenderTargetView* renderTarget,
	const SubTextureOptions& options) {
	CLAIR_ASSERT(renderTarget, "Error creating render target");
	mD3dRenderTargetView = renderTarget;
	mOptions = options;
}

void RenderTarget::terminate() {
	if (mD3dRenderTargetView) {
		mD3dRenderTargetView->Release();
	}
}

void RenderTarget::clear(const Float4& value) {
	auto d3dContext = D3dDevice::getD3dDeviceContext();
	d3dContext->ClearRenderTargetView(mD3dRenderTargetView, &value[0][0]);
}
