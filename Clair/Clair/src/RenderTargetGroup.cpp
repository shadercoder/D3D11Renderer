#include "Clair/RenderTargetGroup.h"
#include "SubTexture.h"

using namespace Clair;

RenderTargetGroup::RenderTargetGroup(const int numRenderTargets)
	: mDepthStencilTarget (nullptr) 
	, mNumRenderTargets (numRenderTargets) {
	CLAIR_ASSERT(mNumRenderTargets >= 1, "Invalid number of render targets");
	mD3dRenderTargets = new ID3D11RenderTargetView*[mNumRenderTargets];
	for (int i {0}; i < mNumRenderTargets; ++i) {
		mD3dRenderTargets[i] = nullptr;
	}
}

RenderTargetGroup::~RenderTargetGroup() {
	delete[] mD3dRenderTargets;
}

void RenderTargetGroup::setRenderTarget(
	const int index, SubTexture* const renderTarget) {
	CLAIR_ASSERT(renderTarget, "Render target should not be null");
	//CLAIR_ASSERT(renderTarget->getType() == Texture::Type::RENDER_TARGET,
	//	"Texture should be of type RENDER_TARGET");
	setD3dRenderTarget(index, renderTarget->getD3dRenderTargetView());
}

// TODO: REMOVE
void RenderTargetGroup::resize(const int width, const int height) {
	for (int i {0}; i < mNumRenderTargets; ++i) {
		//CLAIR_ASSERT(mRenderTargets[i],
		//	"One or more render target Textures missing");
		//mRenderTargets[i]->resize(width, height);
	}
	if (mDepthStencilTarget) {
		CLAIR_ASSERT(mDepthStencilTarget,
			"Depth stencil Texture missing");
		mDepthStencilTarget->resize(width, height);
	}
}
