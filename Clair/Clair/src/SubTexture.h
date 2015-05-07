#pragma once
#include "D3dDevice.h"
#include "Clair/Debug.h"
#include "Clair/Texture.h"

namespace Clair {
	class SubTexture {
	public:
		SubTexture(ID3D11ShaderResourceView* d3dShaderResView,
				   ID3D11RenderTargetView* d3dRenderTargetViews,
				   Texture::Type type);
		~SubTexture();

		ID3D11ShaderResourceView* getD3DShaderResourceView() const;
		ID3D11RenderTargetView* getD3dRenderTargetView() const;
	
	private:
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
		ID3D11RenderTargetView* mD3dRenderTargetView {nullptr};
		Texture::Type mType {Texture::Type::DEFAULT};
	};

	inline ID3D11ShaderResourceView*
	SubTexture::getD3DShaderResourceView() const {
		return mD3dShaderResView;
	}

	inline ID3D11RenderTargetView*
	SubTexture::getD3dRenderTargetView() const {
		return mD3dRenderTargetView;
	}
}