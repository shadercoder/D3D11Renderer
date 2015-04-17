#pragma once

namespace Clair {
	class MaterialConstBufferData {
	public:
		MaterialConstBufferData(
			unsigned sizeVs, unsigned sizeGs, unsigned sizePs);
		~MaterialConstBufferData();

		static void copy(
			MaterialConstBufferData* to,
			const MaterialConstBufferData* from);

		char* getDataVs();
		char* getDataGs();
		char* getDataPs();
		const char* getDataVs() const;
		const char* getDataGs() const;
		const char* getDataPs() const;
		unsigned getSizeVs() const;
		unsigned getSizeGs() const;
		unsigned getSizePs() const;

	private:
		void create(
			unsigned sizeVs, unsigned sizeGs, unsigned sizePs);
		void destroy();

		char* mDataVs {nullptr};
		char* mDataGs {nullptr};
		char* mDataPs {nullptr};
		unsigned mSizeVs {0};
		unsigned mSizeGs {0};
		unsigned mSizePs {0};
	};

	inline char* MaterialConstBufferData::getDataVs() {
		return mDataVs;
	}

	inline char* MaterialConstBufferData::getDataGs() {
		return mDataGs;
	}

	inline char* MaterialConstBufferData::getDataPs() {
		return mDataPs;
	}

	inline const char* MaterialConstBufferData::getDataVs() const {
		return mDataVs;
	}

	inline const char* MaterialConstBufferData::getDataGs() const {
		return mDataGs;
	}

	inline const char* MaterialConstBufferData::getDataPs() const {
		return mDataPs;
	}

	inline unsigned MaterialConstBufferData::getSizeVs() const {
		return mSizeVs;
	}

	inline unsigned MaterialConstBufferData::getSizeGs() const {
		return mSizeGs;
	}

	inline unsigned MaterialConstBufferData::getSizePs() const {
		return mSizePs;
	}
}