#pragma once
#include "clair/vertexLayout.h"

namespace Clair {
	class VertexShader;
	class PixelShader;

	class Material {
	public:
		VertexLayout vertexLayout;
		VertexShader* vertexShader;
		PixelShader* pixelShader;
	};
}
