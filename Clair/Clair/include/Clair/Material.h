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
		explicit Material(const char* data);
		~Material();

		template<typename T>
		T* getConstantBufferPs() const;

		bool isValid() const;
		VertexShader* getVertexShader() const;
		PixelShader* getPixelShader() const;
		ConstantBuffer* getConstantBufferPs() const;
		const MaterialConstBufferData* getConstBufferData() const;

	private:
		VertexLayout mVertexLayout {};
		VertexShader* mVertexShader {nullptr};
		PixelShader* mPixelShader {nullptr};
		ConstantBuffer* mCBufferVs {nullptr};
		ConstantBuffer* mCBufferGs {nullptr};
		ConstantBuffer* mCBufferPs {nullptr};
		MaterialConstBufferData* mCBufferData {};
		bool mIsValid {false};
	};

	template<typename T>
	T* Material::getConstantBufferPs() const {
		CLAIR_ASSERT(sizeof(T) == mCBufferData->getSizePs(),
					 "CBuffer data interpreted as class with wrong size");
		return reinterpret_cast<T*>(mCBufferData->getDataPs());
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

	inline const MaterialConstBufferData*
	Material::getConstBufferData() const {
		return mCBufferData;
	}
}
