#pragma once
#include "Clair/VertexLayout.h"

struct ID3D11Device;

namespace Clair {
	class VertexShader;
	class PixelShader;

	class Material {
	public:
		Material(ID3D11Device* d3dDevice, char* data);
		~Material();

		VertexLayout vertexLayout {};
		VertexShader* vertexShader {nullptr};
		PixelShader* pixelShader {nullptr};
	};
}
