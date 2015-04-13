#pragma once
#include "Clair/Material.h"

namespace Clair {
	class MaterialInstance {
	public:
		~MaterialInstance();

		bool isValid {false};
		Material* material {nullptr};

		template<typename T>
		T* setConstantBuffer();

		const char* getConstantBufferData() const;

	private:
		char* mConstantBufferData {nullptr};
	};

	template<typename T>
	inline T* MaterialInstance::setConstantBuffer() {
		mConstantBufferData = new char[sizeof(T)]();
		return reinterpret_cast<T*>(mConstantBufferData);
	}

	inline const char* MaterialInstance::getConstantBufferData() const {
		return mConstantBufferData;
	}
}
