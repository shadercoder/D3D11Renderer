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
			DEPTH_STENCIL_TARGET,
			CUBE_MAP_DEFAULT,
			CUBE_MAP_RENDER_TARGET
		};
		enum class Format {
			R8G8B8A8_UNORM,
			R32G32B32A32_FLOAT,
			D24_UNORM_S8_UINT,
		};
		struct Options {
			int width {1};
			int height {1};
			const Byte* initialData {nullptr};
			Format format {Format::R8G8B8A8_UNORM};
			Type type {Type::DEFAULT};
			size_t arraySize {1};
			size_t mipLevels {1};
		};

		void initialize(const Options& options);

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

		Options mOptions {};
		bool mIsValid {false};
		ID3D11Texture2D* mD3dTexture {nullptr};
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
		ID3D11RenderTargetView* mD3dRenderTargetView {nullptr};
		ID3D11DepthStencilView* mD3dDepthStencilTargetView {nullptr};
	};

	inline bool Texture::isValid() const {
		return mIsValid;
	}

	inline Texture::Type Texture::getType() const {
		return mOptions.type;
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
		CLAIR_ASSERT(mOptions.type == Type::RENDER_TARGET,
			"Texture is not a render target");
		return mD3dRenderTargetView;
	}
}