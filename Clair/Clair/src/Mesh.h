#pragma once
#include "Clair/VertexLayout.h"

struct ID3D11Device;

namespace Clair {
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class PixelShader;

	class Mesh {
	public:
		Mesh(ID3D11Device* d3dDevice, const char* data);
		~Mesh();

		VertexLayout& getVertexLayout();
		VertexBuffer* getVertexBuffer() const;
		IndexBuffer* getIndexBuffer() const;
		unsigned getIndexBufferSize() const;

	private:
		VertexLayout mVertexLayout {};
		VertexBuffer* mVertexBuffer {nullptr};
		IndexBuffer* mIndexBuffer {nullptr};
		unsigned mIndexBufferSize {0};
	};

	inline VertexLayout& Mesh::getVertexLayout() {
		return mVertexLayout;
	}

	inline VertexBuffer* Mesh::getVertexBuffer() const {
		return mVertexBuffer;
	}

	inline IndexBuffer* Mesh::getIndexBuffer() const {
		return mIndexBuffer;
	}

	inline unsigned Mesh::getIndexBufferSize() const {
		return mIndexBufferSize;
	}

}