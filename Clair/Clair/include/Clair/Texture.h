#pragma once
#include <vector>
#include "Clair/Debug.h"
#include "Clair/Matrix.h"

typedef unsigned char Byte;

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

namespace Clair {
	class SubTexture;
	class Texture {
	public:
		enum class Type {
			DEFAULT,
			RENDER_TARGET,
			DEPTH_STENCIL_TARGET,
		};
		enum class Format {
			R8G8B8A8_UNORM,
			R32G32B32A32_FLOAT,
			D24_UNORM_S8_UINT,
		};
		struct Element {
			size_t arrayIndex;
			size_t mipIndex;
		};
		struct Options {
			int width {1};
			int height {1};
			const Byte* initialData {nullptr};
			Format format {Format::R8G8B8A8_UNORM};
			Type type {Type::DEFAULT};
			bool isCubeMap {false};
			size_t arraySize {1};
			size_t maxMipLevels {1};
		};

		void initialize(const Options& options);

		void clear(const Float4& value, const Element& element = Element{0, 0});
		void clear(float value, const Element& element = Element{0, 0});
		void resize(int width, int height);

		bool isValid() const;
		size_t getNumMipMaps() const;
		Type getType() const;
		ID3D11ShaderResourceView* getD3DShaderResourceView() const;
		ID3D11RenderTargetView* getD3dRenderTargetView(
			const Element& element = Element{0, 0}) const;

	private:
		Texture() = default;
		~Texture();
		void destroyD3dObjects();
		size_t maxPossibleMips(int width, int height, size_t max);
		friend class ResourceManager;
		friend class Renderer;

		Options mOptions {};
		size_t mNumMips {0};
		bool mIsValid {false};
		ID3D11Texture2D* mD3dTexture {nullptr};
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
		std::vector<SubTexture*> mSubTextures {};
		std::vector<ID3D11RenderTargetView*> mD3dRenderTargetViews {};
		ID3D11DepthStencilView* mD3dDepthStencilTargetView {nullptr};
	};

	inline bool Texture::isValid() const {
		return mIsValid;
	}

	inline size_t Texture::getNumMipMaps() const {
		return mNumMips;
	}

	inline Texture::Type Texture::getType() const {
		return mOptions.type;
	}

	inline ID3D11ShaderResourceView*
	Texture::getD3DShaderResourceView() const {
		return mD3dShaderResView;
	}

	inline ID3D11RenderTargetView*
	Texture::getD3dRenderTargetView(const Element& e) const {
		CLAIR_ASSERT(mOptions.type == Type::RENDER_TARGET,
			"Texture is not a render target");
		CLAIR_ASSERT(e.arrayIndex >= 0 && e.arrayIndex < mOptions.arraySize,
			"Invalid array index");
		CLAIR_ASSERT(e.mipIndex >= 0 && e.mipIndex < mNumMips,
			"Invalid mip map index");
		if (!mIsValid) {
			return nullptr;
		}
		return mD3dRenderTargetViews[e.arrayIndex * mNumMips + e.mipIndex];
	}
}