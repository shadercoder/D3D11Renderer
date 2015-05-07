#include "SubTexture.h"

Clair::SubTexture::SubTexture(
	ID3D11ShaderResourceView* const d3dShaderResView,
	ID3D11RenderTargetView* const d3dRenderTargetView,
	const Texture::Type type)
	: mD3dShaderResView {d3dShaderResView}
	, mD3dRenderTargetView {d3dRenderTargetView}
	, mType {type} {
	CLAIR_ASSERT(mD3dShaderResView, "Error creating SubTexture");
	if (mType == Texture::Type::RENDER_TARGET) {
		CLAIR_ASSERT(mD3dRenderTargetView, "Error creating SubTexture");
	}
}

Clair::SubTexture::~SubTexture() {
	if (mD3dRenderTargetView) {
		mD3dRenderTargetView->Release();
	}
	if (mD3dShaderResView) {
		mD3dShaderResView->Release();
	}
}
