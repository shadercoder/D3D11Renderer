#pragma once
#include "Clair/Debug.h"
#include "Clair/Matrix.h"
#include "Clair/Texture.h"

struct ID3D11RenderTargetView;

namespace Clair {
	class RenderTarget {
	public:
		RenderTarget();
		~RenderTarget();

		void clear(const Float4& value);

		ID3D11RenderTargetView* getD3dRenderTargetView() const;

	private:
		friend class Texture;
		void initialize(ID3D11RenderTargetView* renderTarget,
						const SubTextureOptions& options);
		void terminate();
		ID3D11RenderTargetView* mD3dRenderTargetView {nullptr};
		SubTextureOptions mOptions;
	};

	inline ID3D11RenderTargetView*
	RenderTarget::getD3dRenderTargetView() const {
		return mD3dRenderTargetView;
	}
}