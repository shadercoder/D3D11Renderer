#include "Clair/RenderTargetGroup.h"

using namespace Clair;

RenderTargetGroup::RenderTargetGroup(const int numRenderTargets)
	: mDepthStencilTarget (nullptr) 
	, mNumRenderTargets (numRenderTargets) {
	CLAIR_ASSERT(mNumRenderTargets >= 1, "Invalid number of render targets");
	mRenderTargets = new Texture*[mNumRenderTargets];
	mD3dRenderTargetArray = new ID3D11RenderTargetView*[mNumRenderTargets];
	mRenderTargetElements = new Texture::Element[mNumRenderTargets]();
	for (int i {0}; i < mNumRenderTargets; ++i) {
		mRenderTargets[i] = nullptr;
		mD3dRenderTargetArray[i] = nullptr;
	}
}

RenderTargetGroup::~RenderTargetGroup() {
	delete[] mRenderTargetElements;
	delete[] mD3dRenderTargetArray;
	delete[] mRenderTargets;
}

void RenderTargetGroup::resize(const int width, const int height) {
	for (int i {0}; i < mNumRenderTargets; ++i) {
		CLAIR_ASSERT(mRenderTargets[i],
			"One or more render target Textures missing");
		mRenderTargets[i]->resize(width, height);
	}
	if (mDepthStencilTarget) {
		CLAIR_ASSERT(mDepthStencilTarget,
			"Depth stencil Texture missing");
		mDepthStencilTarget->resize(width, height);
	}
}
ID3D11RenderTargetView** RenderTargetGroup::getD3dRenderTargetArray() const {
	for (int i {0}; i < mNumRenderTargets; ++i) {
		const auto targ = mRenderTargets[i]->getD3dRenderTargetView(
			mRenderTargetElements[i]);
		mD3dRenderTargetArray[i] = targ;
	}
	return mD3dRenderTargetArray;
}