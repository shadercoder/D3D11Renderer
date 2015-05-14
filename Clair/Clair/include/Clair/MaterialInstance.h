#pragma once
#include "Clair/Material.h"
#include "Clair/Texture.h"
#include <map>

namespace Clair {
	typedef std::map<unsigned, const ShaderResource*> ShaderResourceMap;
	class MaterialInstance {
	public:
		void initialize(const Material* material);

		void setShaderResource(unsigned index, const ShaderResource* resource);

		bool isValid() const;
		const Material* getMaterial() const;
		const MaterialConstBufferData* getConstBufferData() const;
		const ShaderResourceMap& getShaderResourceMap() const;
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
		ShaderResourceMap mShaderResourceMap;
	};

	template<typename T>
	inline T* MaterialInstance::getConstantBufferPs() const {
		CLAIR_ASSERT(sizeof(T) == mCBufferData->getSizePs(), 
					 "CBuffer data interpreted as class with wrong size");
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

	inline const ShaderResourceMap&
	MaterialInstance::getShaderResourceMap() const {
		return mShaderResourceMap;
	}
}
