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
	class RenderTarget;
	class ShaderResource;
	struct SubTextureOptions {
		size_t arrayStartIndex {0};
		size_t arraySize {1};
		size_t mipStartIndex {0};
		size_t numMips {1};
		bool isCubeMap {false};
	};
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
			R16G16B16A16_FLOAT,
			D24_UNORM_S8_UINT,
			R16G16_FLOAT,
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
			bool generateMips {false};
		};

		void initialize(const Options& options);

		void clear(float value);
		void resize(int width, int height);
		RenderTarget* createSubRenderTarget(
			const SubTextureOptions& options);
		ShaderResource* createSubShaderResource(
			const SubTextureOptions& options);
		void destroyAllSubRenderTargets();
		void destroyAllSubShaderResources();

		bool isValid() const;
		size_t getNumMipMaps() const;
		void getMipMapDimensions(size_t mipLevel, int* outWidth, int* outHeight);
		Type getType() const;
		ShaderResource* getShaderResource() const;
		RenderTarget* getRenderTarget() const;

	private:
		friend class ResourceManager;
		friend class Renderer;
		Texture();
		~Texture();
		void destroyD3dObjects();
		size_t maxPossibleMips(int width, int height, size_t max);
		void initializeSubShaderRes(ShaderResource* outResource,
								   const SubTextureOptions& options);
		void initializeSubRenderTarget(RenderTarget* outTarget,
									  const SubTextureOptions& options);

		Options mOptions {};
		size_t mNumMips {0};
		bool mIsValid {false};
		ID3D11Texture2D* mD3dTexture {nullptr};
		ID3D11DepthStencilView* mD3dDepthStencilTargetView {nullptr};
		std::vector<ShaderResource*> mSubShaderResources {};
		std::vector<RenderTarget*> mSubRenderTargets {};
		ShaderResource* mShaderResource {nullptr};
		RenderTarget* mRenderTarget {nullptr};
		unsigned mD3dFormat {0};
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

	inline ShaderResource*
	Texture::getShaderResource() const {
		return mShaderResource;
	}

	inline RenderTarget*
	Texture::getRenderTarget() const {
		CLAIR_ASSERT(mOptions.type == Type::RENDER_TARGET,
			"Texture is not a render target");
		if (!mIsValid) {
			return nullptr;
		}
		return mRenderTarget;
	}
}