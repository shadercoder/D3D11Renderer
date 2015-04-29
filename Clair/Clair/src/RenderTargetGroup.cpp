#include "Clair/RenderTargetGroup.h"

using namespace Clair;

RenderTargetGroup::RenderTargetGroup(const int numRenderTargets)
	: mDepthStencilTarget (nullptr) 
	, mNumRenderTargets (numRenderTargets) {
	CLAIR_ASSERT(mNumRenderTargets >= 1, "Invalid number of render targets");
	mRenderTargets = new RenderTarget*[mNumRenderTargets];
	mD3dRenderTargetArray = new ID3D11RenderTargetView*[mNumRenderTargets];
	for (int i {0}; i < mNumRenderTargets; ++i) {
		mRenderTargets[i] = nullptr;
		mD3dRenderTargetArray[i] = nullptr;
	}
}

RenderTargetGroup::~RenderTargetGroup() {
	delete[] mD3dRenderTargetArray;
	delete[] mRenderTargets;
}

void RenderTargetGroup::resize(const int width, const int height) {
	for (int i {0}; i < mNumRenderTargets; ++i) {
		CLAIR_ASSERT(mRenderTargets[i]->getTexture(),
			"One or more RenderTargets has no texture to resize");
		mRenderTargets[i]->getTexture()->resize(width, height);
	}
	if (mDepthStencilTarget) {
		CLAIR_ASSERT(mDepthStencilTarget->getTexture(),
			"The DepthStencilTarget has no texture to resize");
		mDepthStencilTarget->getTexture()->resize(width, height);
	}
}