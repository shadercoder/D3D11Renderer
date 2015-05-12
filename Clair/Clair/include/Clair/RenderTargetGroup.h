#pragma once
#include "Clair/Texture.h"
#include "Clair/RenderTarget.h"
#include "Clair/Debug.h"

namespace Clair {
	class RenderTargetGroup {
	public:
		explicit RenderTargetGroup(int numRenderTargets);
		~RenderTargetGroup();

		void setRenderTarget(int index, RenderTarget* renderTarget);
		void setDepthStencilTarget(Texture* depthStencilTarget);
		int getNumRenderTargets() const;
		Texture* getDepthStencilTarget() const;
		ID3D11RenderTargetView** getD3dRenderTargets() const;

	private:
		ID3D11RenderTargetView** mD3dRenderTargets {nullptr};
		RenderTarget** mRenderTargets {nullptr};
		Texture* mDepthStencilTarget {nullptr};
		int mNumRenderTargets {0};
	};

	inline int RenderTargetGroup::getNumRenderTargets() const {
		return mNumRenderTargets;
	}

	inline void RenderTargetGroup::setDepthStencilTarget(
		Texture* const depthStencilTarget) {
		CLAIR_ASSERT(depthStencilTarget,
			"Depth/stencil target should not be null");
		CLAIR_ASSERT(depthStencilTarget->getType() ==
			Texture::Type::DEPTH_STENCIL_TARGET,
			"Texture should be of type DEPTH_STENCIL_TARGET");
		CLAIR_DEBUG_LOG_IF(mDepthStencilTarget,
			"Overwriting existing depth/stencil target in RenderTargetGroup");
		mDepthStencilTarget = depthStencilTarget;
	}

	inline Texture*
	RenderTargetGroup::getDepthStencilTarget() const {
		return mDepthStencilTarget;
	}

	inline ID3D11RenderTargetView**
	RenderTargetGroup::getD3dRenderTargets() const {
		for (int i {0}; i < mNumRenderTargets; ++i) {
			mD3dRenderTargets[i] = mRenderTargets[i]->getD3dRenderTargetView();
		}
		return mD3dRenderTargets;
	}

	inline void RenderTargetGroup::setRenderTarget(
		const int index, RenderTarget* const target) {
		CLAIR_ASSERT(index >= 0 && index < mNumRenderTargets, "Invalid index");
		CLAIR_ASSERT(target, "Render target should not be null");
		mRenderTargets[index] = target;
	}
}