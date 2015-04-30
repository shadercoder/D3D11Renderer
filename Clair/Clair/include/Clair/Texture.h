#pragma once
#include "Clair/Debug.h"
#include "Clair/Matrix.h"

typedef unsigned char Byte;

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

namespace Clair {
	class Texture {
	public:
		enum class Type {
			DEFAULT,
			RENDER_TARGET,
			DEPTH_STENCIL_TARGET
		};
		enum class Format {
			R8G8B8A8_UNORM,
			R32G32B32A32_FLOAT,
			D24_UNORM_S8_UINT,
		};

		void initialize(int width, int height,
						const Byte* data,
						Format format = Format::R8G8B8A8_UNORM,
						Type option = Type::DEFAULT);

		void clear(const Float4& value);
		void clear(float value);
		void resize(int width, int height);


		bool isValid() const;
		Type getType() const;
		const ID3D11Texture2D* getD3dTexture() const;
		ID3D11ShaderResourceView* getD3DShaderResourceView() const;
		ID3D11RenderTargetView* getD3dRenderTargetView() const;

	private:
		Texture() = default;
		~Texture();
		void destroyD3dObjects();
		friend class ResourceManager;
		friend class Renderer;

		bool mIsValid {false};
		Format mFormat {Format::R8G8B8A8_UNORM};
		Type mType {Type::DEFAULT};
		ID3D11Texture2D* mD3dTexture {nullptr};
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
		ID3D11RenderTargetView* mD3dRenderTargetView {nullptr};
		ID3D11DepthStencilView* mD3dDepthStencilTargetView {nullptr};
	};

	inline bool Texture::isValid() const {
		return mIsValid;
	}

	inline Texture::Type Texture::getType() const {
		return mType;
	}

	inline const ID3D11Texture2D* Texture::getD3dTexture() const {
		return mD3dTexture;
	}

	inline ID3D11ShaderResourceView*
	Texture::getD3DShaderResourceView() const {
		return mD3dShaderResView;
	}

	inline ID3D11RenderTargetView*
	Texture::getD3dRenderTargetView() const {
		CLAIR_ASSERT(mType == Type::RENDER_TARGET,
			"Texture is not a render target");
		return mD3dRenderTargetView;
	}
}