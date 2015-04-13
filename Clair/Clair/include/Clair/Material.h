#pragma once
#include "Clair/VertexLayout.h"

struct ID3D11Device;

namespace Clair {
	class VertexShader;
	class PixelShader;
	class ConstantBuffer;

	class Material {
	public:
		Material(ID3D11Device* d3dDevice, const char* data);
		~Material();

		// Sets constant buffer type, creates data and returns pointer to it.
		template<typename T>
		T* setConstantBuffer();

		bool isValid() const;
		VertexShader* getVertexShader() const;
		PixelShader* getPixelShader() const;
		ConstantBuffer* getConstantBuffer() const;
		const char* getConstantBufferData() const;

	private:
		char* createConstantBuffer(size_t size);

		bool mIsValid {false};
		VertexLayout mVertexLayout {};
		VertexShader* mVertexShader {nullptr};
		PixelShader* mPixelShader {nullptr};
		ConstantBuffer* mConstantBuffer {nullptr};
		char* mConstantBufferData {nullptr};
	};

	template<typename T>
	T* Material::setConstantBuffer() {
		return reinterpret_cast<T*>(createConstantBuffer(sizeof(T)));
	}

	inline bool Material::isValid() const {
		return mIsValid;
	}

	inline VertexShader* Material::getVertexShader() const {
		return mVertexShader;
	}

	inline PixelShader* Material::getPixelShader() const {
		return mPixelShader;
	}

	inline ConstantBuffer* Material::getConstantBuffer() const {
		return mConstantBuffer;
	}

	inline const char* Material::getConstantBufferData() const {
		return mConstantBufferData;
	}
}
