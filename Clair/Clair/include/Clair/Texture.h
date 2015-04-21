#pragma once
#include "Clair/Matrix.h"

typedef unsigned char Byte;

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;

namespace Clair {
	class Texture {
	public:
		void initialize(int width, int height,
						const Byte* data, bool isRenderTarget = false);
		void clearRenderTarget(const Float4& color);

		bool isValid() const;
		bool isRenderTarget() const;
		const ID3D11Texture2D* getD3dTexture() const;
		ID3D11ShaderResourceView* getD3DShaderResourceView() const;

	private:
		Texture() = default;
		~Texture();
		friend class ResourceManager;
		friend class LowLevelRenderer;

		bool mIsValid {false};
		bool mIsRenderTarget {false};
		ID3D11Texture2D* mD3dTexture {nullptr};
		ID3D11RenderTargetView* mD3dRenderTargetView {nullptr};
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
	};

	inline bool Texture::isValid() const {
		return mIsValid;
	}

	inline bool Texture::isRenderTarget() const {
		return mIsRenderTarget;
	}

	inline const ID3D11Texture2D* Texture::getD3dTexture() const {
		return mD3dTexture;
	}

	inline ID3D11ShaderResourceView*
	Texture::getD3DShaderResourceView() const {
		return mD3dShaderResView;
	}
}