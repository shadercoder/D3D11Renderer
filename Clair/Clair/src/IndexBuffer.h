#pragma once

struct ID3D11Buffer;

namespace Clair {
	class IndexBuffer {
	public:
		IndexBuffer(const unsigned* bufferData, unsigned bufferSize);
		~IndexBuffer();

		bool isValid() const;
		ID3D11Buffer* getD3dBuffer() const;

	private:
		bool mIsValid {false};
		ID3D11Buffer* mD3dBuffer {nullptr};
	};

	inline bool IndexBuffer::isValid() const {
		return mIsValid;
	}

	inline ID3D11Buffer* IndexBuffer::getD3dBuffer() const {
		return mD3dBuffer;
	}
}