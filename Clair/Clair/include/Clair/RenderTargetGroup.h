#pragma once
#include "Clair/RenderTarget.h"
#include "Clair/DepthStencilTarget.h"
#include "Clair/Debug.h"

namespace Clair {
	class RenderTargetGroup {
	public:
		explicit RenderTargetGroup(int numRenderTargets);
		~RenderTargetGroup();

		void setRenderTarget(int index, RenderTarget* renderTarget);
		void setDepthStencilTarget(DepthStencilTarget* depthStencilTarget);
		int getNumRenderTargets() const;
		RenderTarget* getRenderTarget(int index) const;
		DepthStencilTarget* getDepthStencilTarget() const;
		ID3D11RenderTargetView** getD3dRenderTargetArray() const;

	private:
		RenderTarget** mRenderTargets {nullptr};
		DepthStencilTarget* mDepthStencilTarget {nullptr};
		ID3D11RenderTargetView** mD3dRenderTargetArray {nullptr};
		int mNumRenderTargets {0};
	};

	inline int RenderTargetGroup::getNumRenderTargets() const {
		return mNumRenderTargets;
	}

	inline void RenderTargetGroup::setRenderTarget(
		const int index, RenderTarget* const renderTarget) {
		CLAIR_ASSERT(index >= 0 && index < mNumRenderTargets, "Invalid index");
		CLAIR_ASSERT(renderTarget, "Render target should not be null");
		CLAIR_DEBUG_LOG_IF(mRenderTargets[index],
			"Overwriting existing render target in RenderTargetGroup");
		mRenderTargets[index] = renderTarget;
		mD3dRenderTargetArray[index] = renderTarget->getD3dRenderTargetView();
	}

	inline void RenderTargetGroup::setDepthStencilTarget(
		DepthStencilTarget* const depthStencilTarget) {
		CLAIR_ASSERT(depthStencilTarget,
			"Depth/stencil target should not be null");
		CLAIR_DEBUG_LOG_IF(mDepthStencilTarget,
			"Overwriting existing depth/stencil target in RenderTargetGroup");
		mDepthStencilTarget = depthStencilTarget;
	}

	inline RenderTarget*
	RenderTargetGroup::getRenderTarget(const int index) const {
		CLAIR_ASSERT(index >= 0 && index < mNumRenderTargets,
			"Invalid index");
		return mRenderTargets[index];
	}

	inline DepthStencilTarget*
	RenderTargetGroup::getDepthStencilTarget() const {
		return mDepthStencilTarget;
	}

	inline ID3D11RenderTargetView**
	RenderTargetGroup::getD3dRenderTargetArray() const {
		return mD3dRenderTargetArray;
	}
}