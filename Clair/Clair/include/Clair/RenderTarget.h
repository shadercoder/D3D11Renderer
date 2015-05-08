#pragma once
#include "Clair/Debug.h"
#include "Clair/Matrix.h"

struct ID3D11RenderTargetView;

namespace Clair {
	class RenderTarget {
	public:
		explicit RenderTarget(ID3D11RenderTargetView* renderTarget);
		~RenderTarget();

		void clear(const Float4& value);

		ID3D11RenderTargetView* getD3dRenderTargetView() const;

	private:
		ID3D11RenderTargetView* mD3dRenderTargetView {nullptr};
	};

	inline
	RenderTarget::RenderTarget(ID3D11RenderTargetView* const renderTarget) {
		CLAIR_ASSERT(renderTarget, "Error creating render target");
		mD3dRenderTargetView = renderTarget;
	}

	inline ID3D11RenderTargetView*
	RenderTarget::getD3dRenderTargetView() const {
		return mD3dRenderTargetView;
	}
}