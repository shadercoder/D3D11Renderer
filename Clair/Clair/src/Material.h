#pragma once
#include "Clair/VertexLayout.h"

struct ID3D11Device;

namespace Clair {
	class VertexShader;
	class PixelShader;

	class Material {
	public:
		Material(ID3D11Device* d3dDevice, const char* data);
		~Material();

		VertexShader* getVertexShader() const;
		PixelShader* getPixelShader() const;

	private:
		VertexLayout mVertexLayout {};
		VertexShader* mVertexShader {nullptr};
		PixelShader* mPixelShader {nullptr};
	};

	inline VertexShader* Material::getVertexShader() const {
		return mVertexShader;
	}

	inline PixelShader* Material::getPixelShader() const {
		return mPixelShader;
	}
}
