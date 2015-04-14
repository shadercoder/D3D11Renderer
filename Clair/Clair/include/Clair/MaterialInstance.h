#pragma once
#include "Clair/Material.h"

namespace Clair {
	class MaterialInstance {
	public:
		explicit MaterialInstance(const Material* material);

		template<typename T>
		T* getConstantBufferPs();

		bool isValid() const;
		const Material* getMaterial() const;
		const MaterialConstBufferData* getConstBufferData() const;

	private:
		MaterialConstBufferData mCBufferData;
		const Material* mMaterial {nullptr};
		bool mIsValid {false};
	};

	template<typename T>
	inline T* MaterialInstance::getConstantBufferPs() {
		CLAIR_ASSERT(sizeof(T) == mCBufferData.sizePs, 
					 "CBuffer data interpreted as class with wrong size");
		mCBufferData.dataPs = new char[sizeof(T)]();
		return reinterpret_cast<T*>(mCBufferData.dataPs);
	}

	inline bool MaterialInstance::isValid() const {
		return mIsValid;
	}

	inline const Material* MaterialInstance::getMaterial() const {
		return mMaterial;
	}

	inline const MaterialConstBufferData*
	MaterialInstance::getConstBufferData() const {
		return &mCBufferData;
	}
}
