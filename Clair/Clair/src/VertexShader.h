#pragma once

typedef unsigned char Byte;

struct ID3D11VertexShader;

namespace Clair {
	class VertexShader {
	public:
		VertexShader(const Byte* byteCode,
					 unsigned byteCodeSize);
		~VertexShader();

		bool isValid() const;
		const Byte* getByteCode() const;
		size_t getByteCodeSize() const;
		ID3D11VertexShader* getD3dShader() const;

	private:
		bool mIsValid {false};
		Byte* mByteCode {nullptr};
		size_t mByteCodeSize {0};
		ID3D11VertexShader* mD3dShader {nullptr};
	};

	inline bool VertexShader::isValid() const {
		return mIsValid;
	}

	inline const Byte* VertexShader::getByteCode() const {
		return mByteCode;
	}

	inline size_t VertexShader::getByteCodeSize() const {
		return mByteCodeSize;
	}

	inline ID3D11VertexShader* VertexShader::getD3dShader() const {
		return mD3dShader;
	}
}