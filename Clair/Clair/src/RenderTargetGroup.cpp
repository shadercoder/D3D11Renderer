#include "Clair/RenderTargetGroup.h"

using namespace Clair;

RenderTargetGroup::RenderTargetGroup(const int numRenderTargets)
	: mDepthStencilTarget (nullptr) 
	, mNumRenderTargets (numRenderTargets) {
	CLAIR_ASSERT(mNumRenderTargets >= 1, "Invalid number of render targets");
	mRenderTargets = new RenderTarget*[mNumRenderTargets];
	for (int i {0}; i < mNumRenderTargets; ++i) {
		mRenderTargets[i] = nullptr;
	}
}

RenderTargetGroup::~RenderTargetGroup() {
	delete[] mRenderTargets;
}