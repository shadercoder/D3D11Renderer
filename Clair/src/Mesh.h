#pragma once
#include "Clair/VertexLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Clair {
	class VertexShader;
	class PixelShader;

	//typedef std::vector<unsigned> VertexLayoutOffsets;

	class Mesh {
	public:
		VertexLayout vertexLayout {};
		//VertexLayoutOffsets vertexLayoutOffsets {};
		VertexBuffer* vertexBuffer {nullptr};
		IndexBuffer* indexBuffer {nullptr};
		unsigned indexBufferSize {0};
	};
}