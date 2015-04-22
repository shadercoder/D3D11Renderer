#pragma once
#include "Clair/Texture.h"
#include "Clair/Matrix.h"

struct ID3D11RenderTargetView;

namespace Clair {
	class RenderTarget {
	public:
		void initialize(Texture* texture);

		void RenderTarget::clear(const Float4& color);
	
		bool isValid() const;
	private:
		RenderTarget() = default;
		~RenderTarget();
		friend class ResourceManager;
		friend class Renderer;
		bool mIsValid {false};
		Texture* mTexture {nullptr};
		ID3D11RenderTargetView* mD3dRenderTargetView {nullptr};
	};

	inline bool RenderTarget::isValid() const {
		return mIsValid;
	}
}