#pragma once
#include "Clair/Texture.h"

struct ID3D11DepthStencilView;

namespace Clair {
	class DepthStencilTarget {
	public:
		void initialize(Texture* texture);
		
		void clearDepth(float value);

		bool isValid() const;

	private:
		DepthStencilTarget() = default;
		~DepthStencilTarget();
		friend class ResourceManager;
		friend class Renderer;
		bool mIsValid {false};
		Texture* mTexture {nullptr};
		ID3D11DepthStencilView* mD3dDepthStencilTargetView {nullptr};
	};

	inline bool DepthStencilTarget::isValid() const {
		return mIsValid;
	}
}