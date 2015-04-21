#pragma once
#include "Clair/Material.h"
#include "Clair/Texture.h"
#include <map>

namespace Clair {
	typedef std::map<unsigned, const Texture*> TextureMap;
	class MaterialInstance {
	public:
		void initialize(const Material* material);

		void setTexture(unsigned index, const Texture* texture);

		bool isValid() const;
		const Material* getMaterial() const;
		const MaterialConstBufferData* getConstBufferData() const;
		const TextureMap& getTextureMap() const;
		template<typename T>
		T* getConstantBufferPs() const;

	private:
		MaterialInstance() = default;
		~MaterialInstance();
		friend class ResourceManager;
		friend class Object;
		bool mIsValid {false};
		const Material* mMaterial {nullptr};
		MaterialConstBufferData* mCBufferData;
		TextureMap mTextureMap;
	};

	template<typename T>
	inline T* MaterialInstance::getConstantBufferPs() const {
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

	inline const TextureMap& MaterialInstance::getTextureMap() const {
		return mTextureMap;
	}
}
