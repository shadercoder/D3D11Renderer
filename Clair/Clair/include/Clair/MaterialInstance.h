#pragma once
#include "Clair/Material.h"

namespace Clair {
	class MaterialInstance {
	public:
		explicit MaterialInstance(const Material* material);
		~MaterialInstance();

		template<typename T>
		T* getConstantBufferPs();

		bool isValid() const;
		const Material* getMaterial() const;
		const MaterialConstBufferData* getConstBufferData() const;

	private:
		const Material* mMaterial {nullptr};
		MaterialConstBufferData* mCBufferData;
		bool mIsValid {false};
	};

	template<typename T>
	inline T* MaterialInstance::getConstantBufferPs() {
		CLAIR_ASSERT(sizeof(T) == mCBufferData->getSizePs(), 
					 "CBuffer data interpreted as class with wrong size");
		//mCBufferData->getDataPs() = new char[sizeof(T)]();
		return reinterpret_cast<T*>(mCBufferData->getDataPs());
	}

	inline bool MaterialInstance::isValid() const {
		return mIsValid;
	}

	inline const Material* MaterialInstance::getMaterial() const {
		return mMaterial;
	}

	inline const MaterialConstBufferData*
	MaterialInstance::getConstBufferData() const {
		return mCBufferData;
	}
}
