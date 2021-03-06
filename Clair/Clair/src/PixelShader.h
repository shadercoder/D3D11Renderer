#pragma once

typedef unsigned char Byte;

struct ID3D11PixelShader;

namespace Clair {
	class PixelShader {
	public:
		PixelShader(const Byte* byteCode,
					unsigned byteCodeSize);
		~PixelShader();

		bool isValid() const;
		ID3D11PixelShader* getD3dShader() const;

	private:
		bool mIsValid {false};
		ID3D11PixelShader* mD3dShader {nullptr};
	};

	inline bool PixelShader::isValid() const {
		return mIsValid;
	}

	inline ID3D11PixelShader* PixelShader::getD3dShader() const {
		return mD3dShader;
	}
}