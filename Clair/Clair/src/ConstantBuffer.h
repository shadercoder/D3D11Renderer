#pragma once

struct ID3D11Buffer;

namespace Clair {
	class ConstantBuffer {
	public:
		explicit ConstantBuffer(size_t size);
		~ConstantBuffer();

		bool isValid() const;
		ID3D11Buffer* getD3dBuffer() const;

	private:
		bool mIsValid {false};
		ID3D11Buffer* mD3dBuffer {nullptr};
	};

	inline bool ConstantBuffer::isValid() const {
		return mIsValid;
	}

	inline ID3D11Buffer* ConstantBuffer::getD3dBuffer() const {
		return mD3dBuffer;
	}
}