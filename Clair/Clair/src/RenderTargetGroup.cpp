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
