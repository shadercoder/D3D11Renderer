#pragma once

typedef unsigned char Byte;

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace Clair {
	class Texture {
	public:
		explicit Texture(const Byte* data);
		~Texture();

		bool isValid() const;
		const ID3D11Texture2D* getD3dTexture() const;
		ID3D11ShaderResourceView* getD3DShaderResourceView() const;

	private:
		bool mIsValid {false};
		ID3D11Texture2D* mD3dTexture {nullptr};
		ID3D11ShaderResourceView* mD3dShaderResView {nullptr};
	};

	inline bool Texture::isValid() const {
		return mIsValid;
	}

	inline const ID3D11Texture2D* Texture::getD3dTexture() const {
		return mD3dTexture;
	}

	inline ID3D11ShaderResourceView*
	Texture::getD3DShaderResourceView() const {
		return mD3dShaderResView;
	}
}