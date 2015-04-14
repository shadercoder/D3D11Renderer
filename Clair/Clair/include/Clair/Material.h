#pragma once
#include "Clair/VertexLayout.h"
#include "Clair/MaterialConstBufferData.h"
#include "Clair/Debug.h"

namespace Clair {
	class VertexShader;
	class PixelShader;
	class ConstantBuffer;

	class Material {
	public:
		Material(const char* data);
		~Material();

		template<typename T>
		T* getConstantBufferPs();

		bool isValid() const;
		VertexShader* getVertexShader() const;
		PixelShader* getPixelShader() const;
		ConstantBuffer* getConstantBufferPs() const;
		const MaterialConstBufferData* getConstBufferData() const;

	private:
		MaterialConstBufferData mCBufferData {};
		VertexLayout mVertexLayout {};
		VertexShader* mVertexShader {nullptr};
		PixelShader* mPixelShader {nullptr};
		ConstantBuffer* mCBufferVs {nullptr};
		ConstantBuffer* mCBufferGs {nullptr};
		ConstantBuffer* mCBufferPs {nullptr};
		bool mIsValid {false};
	};

	template<typename T>
	T* Material::getConstantBufferPs() {
		CLAIR_ASSERT(sizeof(T) == mCBufferData.sizePs,
					 "CBuffer data interpreted as class with wrong size");
		return reinterpret_cast<T*>(mCBufferData.dataPs);
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

	inline ConstantBuffer* Material::getConstantBufferPs() const {
		return mCBufferPs;
	}

	inline const MaterialConstBufferData* Material::getConstBufferData() const {
		return &mCBufferData;
	}
}
