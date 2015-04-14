#pragma once

struct ID3D11Buffer;

namespace Clair {
	class VertexBuffer {
	public:
		VertexBuffer(const char* bufferData,
					 unsigned bufferSize);
		~VertexBuffer();

		bool isValid() const;
		ID3D11Buffer* getD3dBuffer() const;

	private:
		bool mIsValid {false};
		ID3D11Buffer* mD3dBuffer {nullptr};
	};

	inline bool VertexBuffer::isValid() const {
		return mIsValid;
	}

	inline ID3D11Buffer* VertexBuffer::getD3dBuffer() const {
		return mD3dBuffer;
	}
}