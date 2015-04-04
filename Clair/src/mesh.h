#pragma once
#include "clair/vertexLayout.h"

struct ID3D11Buffer;

namespace Clair {
	class VertexShader;
	class PixelShader;

	typedef std::vector<unsigned> VertexLayoutOffsets;

	class Mesh {
	public:
		VertexLayout vertexLayout {};
		VertexLayoutOffsets vertexLayoutOffsets {};
		ID3D11Buffer* vertexBuffer {nullptr};
		ID3D11Buffer* indexBuffer {nullptr};
		unsigned indexBufferSize {0};
	};
}