#pragma once
#include "Clair/VertexLayout.h"

namespace Clair {
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class PixelShader;

	class Mesh {
	public:
		explicit Mesh(const Byte* data);
		~Mesh();

		bool isValid() const;
		VertexLayout& getVertexLayout();
		VertexBuffer* getVertexBuffer() const;
		IndexBuffer* getIndexBuffer() const;
		unsigned getIndexBufferSize() const;

	private:
		VertexLayout mVertexLayout {};
		VertexBuffer* mVertexBuffer {nullptr};
		IndexBuffer* mIndexBuffer {nullptr};
		unsigned mIndexBufferSize {0};
		bool mIsValid {false};
	};

	inline bool Mesh::isValid() const {
		return mIsValid;
	}

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