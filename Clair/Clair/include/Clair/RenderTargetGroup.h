#pragma once
#include "Clair/Texture.h"
#include "Clair/Debug.h"

namespace Clair {
	class RenderTargetGroup {
	public:
		explicit RenderTargetGroup(int numRenderTargets);
		~RenderTargetGroup();

		void resize(int width, int height);

		void setRenderTarget(int index, Texture* renderTarget);
		void setRenderTarget(int index, SubTexture* renderTarget);
		void setDepthStencilTarget(Texture* depthStencilTarget);
		int getNumRenderTargets() const;
		Texture* getDepthStencilTarget() const;
		ID3D11RenderTargetView** getD3dRenderTargets() const;

	private:
		void setD3dRenderTarget(int index, ID3D11RenderTargetView* d3dTarget);
		ID3D11RenderTargetView** mD3dRenderTargets {nullptr};
		Texture* mDepthStencilTarget {nullptr};
		int mNumRenderTargets {0};
	};

	inline int RenderTargetGroup::getNumRenderTargets() const {
		return mNumRenderTargets;
	}

	inline void RenderTargetGroup::setRenderTarget(
		const int index, Texture* const renderTarget) {
		CLAIR_ASSERT(renderTarget, "Render target should not be null");
		CLAIR_ASSERT(renderTarget->getType() == Texture::Type::RENDER_TARGET,
			"Texture should be of type RENDER_TARGET");
		setD3dRenderTarget(index, renderTarget->getD3dRenderTargetView());
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
		return mD3dRenderTargets;
	}

	inline void RenderTargetGroup::setD3dRenderTarget(
		const int index, ID3D11RenderTargetView* const d3dTarget) {
		CLAIR_ASSERT(index >= 0 && index < mNumRenderTargets, "Invalid index");
		CLAIR_ASSERT(d3dTarget, "Render target should not be null");
		CLAIR_DEBUG_LOG_IF(mD3dRenderTargets[index],
			"Overwriting existing render target in RenderTargetGroup");
		mD3dRenderTargets[index] = d3dTarget;
	}
}