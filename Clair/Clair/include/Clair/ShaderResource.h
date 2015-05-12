#pragma once
#include "Clair/Debug.h"
#include "Clair/Texture.h"

struct ID3D11ShaderResourceView;

namespace Clair {
	class ShaderResource {
	public:
		ShaderResource();
		~ShaderResource();

		ID3D11ShaderResourceView* getD3dShaderResourceView() const;

	private:
		friend class Texture;
		void initialize(ID3D11ShaderResourceView* shaderResource,
						const SubTextureOptions& options);
		void terminate();
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
		SubTextureOptions mOptions {};
	};

	inline ID3D11ShaderResourceView*
	ShaderResource::getD3dShaderResourceView() const {
		return mD3dShaderResView;
	}
}