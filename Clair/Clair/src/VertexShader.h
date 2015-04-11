#pragma once

struct ID3D11Device;
struct ID3D11VertexShader;

namespace Clair {
	class VertexShader {
	public:
		VertexShader(ID3D11Device* d3dDevice,
					 const char* byteCode,
					 unsigned byteCodeSize);
		~VertexShader();

		bool isValid() const;
		const char* getByteCode() const;
		size_t getByteCodeSize() const;
		ID3D11VertexShader* getD3dShader() const;

	private:
		bool mIsValid {false};
		const char* mByteCode {nullptr};
		size_t mByteCodeSize {0};
		ID3D11VertexShader* mD3dShader {nullptr};
	};

	inline bool VertexShader::isValid() const {
		return mIsValid;
	}

	inline const char* VertexShader::getByteCode() const {
		return mByteCode;
	}

	inline size_t VertexShader::getByteCodeSize() const {
		return mByteCodeSize;
	}

	inline ID3D11VertexShader* VertexShader::getD3dShader() const {
		return mD3dShader;
	}
}