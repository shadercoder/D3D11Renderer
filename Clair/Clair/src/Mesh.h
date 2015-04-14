#pragma once
#include "Clair/VertexLayout.h"

namespace Clair {
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class PixelShader;

	class Mesh {
	public:
		Mesh(const char* data);
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