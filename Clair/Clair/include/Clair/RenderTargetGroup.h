#pragma once
#include "Clair/Texture.h"
#include "Clair/Debug.h"

namespace Clair {
	class RenderTargetGroup {
	public:
		explicit RenderTargetGroup(int numRenderTargets);
		~RenderTargetGroup();

		void resize(int width, int height);

		void setRenderTarget(int index, Texture* renderTarget,
			const Texture::Element& element = Texture::Element{0, 0});
		void setDepthStencilTarget(Texture* depthStencilTarget);
		int getNumRenderTargets() const;
		Texture* getRenderTarget(int index) const;
		Texture* getDepthStencilTarget() const;
		ID3D11RenderTargetView** getD3dRenderTargetArray() const;

	private:
		Texture** mRenderTargets {nullptr};
		Texture* mDepthStencilTarget {nullptr};
		ID3D11RenderTargetView** mD3dRenderTargetArray {nullptr};
		Texture::Element* mRenderTargetElements {nullptr};
		int mNumRenderTargets {0};
	};

	inline int RenderTargetGroup::getNumRenderTargets() const {
		return mNumRenderTargets;
	}

	inline void RenderTargetGroup::setRenderTarget(
		const int index, Texture* const renderTarget,
		const Texture::Element& element) {
		CLAIR_ASSERT(index >= 0 && index < mNumRenderTargets, "Invalid index");
		CLAIR_ASSERT(renderTarget, "Render target should not be null");
		CLAIR_ASSERT(renderTarget->getType() == Texture::Type::RENDER_TARGET
			|| renderTarget->getType() == Texture::Type::CUBE_MAP_RENDER_TARGET,
			"Texture should be of type RENDER_TARGET");
		CLAIR_DEBUG_LOG_IF(mRenderTargets[index],
			"Overwriting existing render target in RenderTargetGroup");
		mRenderTargets[index] = renderTarget;
		mRenderTargetElements[index] = element;
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
	RenderTargetGroup::getRenderTarget(const int index) const {
		CLAIR_ASSERT(index >= 0 && index < mNumRenderTargets,
			"Invalid index");
		return mRenderTargets[index];
	}

	inline Texture*
	RenderTargetGroup::getDepthStencilTarget() const {
		return mDepthStencilTarget;
	}
}