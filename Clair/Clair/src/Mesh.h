#pragma once
#include "Clair/VertexLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Clair {
	class VertexShader;
	class PixelShader;

	class Mesh {
	public:
	//	Mesh(ID3D11Device* d3dDevice, const char* data);

	//private:
		VertexLayout vertexLayout {};
		VertexBuffer* vertexBuffer {nullptr};
		IndexBuffer* indexBuffer {nullptr};
		unsigned indexBufferSize {0};
	};
}