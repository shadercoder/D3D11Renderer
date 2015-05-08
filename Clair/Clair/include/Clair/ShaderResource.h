#pragma once
#include "Clair/Debug.h"

struct ID3D11ShaderResourceView;

namespace Clair {
	class ShaderResource {
	public:
		explicit ShaderResource(ID3D11ShaderResourceView* shaderResource);
		~ShaderResource();

		ID3D11ShaderResourceView* getD3dShaderResourceView() const;

	private:
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
	};

	inline ShaderResource::ShaderResource(
		ID3D11ShaderResourceView* const shaderResource) {
		CLAIR_ASSERT(shaderResource, "Error creating render target");
		mD3dShaderResView = shaderResource;
	}

	inline ID3D11ShaderResourceView*
	ShaderResource::getD3dShaderResourceView() const {
		return mD3dShaderResView;
	}
}