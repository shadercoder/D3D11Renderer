#pragma once

typedef unsigned char Byte;

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

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

		void resize(int width, int height);

		bool isValid() const;
		Type getType() const;
		const ID3D11Texture2D* getD3dTexture() const;
		ID3D11ShaderResourceView* getD3DShaderResourceView() const;

	private:
		Texture() = default;
		~Texture();
		void destroyD3dObjects();
		friend class ResourceManager;
		friend class Renderer;
		friend class RenderTarget;
		friend class DepthStencilTarget;

		bool mIsValid {false};
		Format mFormat {Format::R8G8B8A8_UNORM};
		Type mType {Type::DEFAULT};
		ID3D11Texture2D* mD3dTexture {nullptr};
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
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
}