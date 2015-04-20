#pragma once
typedef unsigned char Byte;

namespace Clair {
	class MaterialConstBufferData {
	public:
		MaterialConstBufferData(
			unsigned sizeVs, unsigned sizeGs, unsigned sizePs);
		~MaterialConstBufferData();

		static void copy(
			MaterialConstBufferData* to,
			const MaterialConstBufferData* from);

		Byte* getDataVs();
		Byte* getDataGs();
		Byte* getDataPs();
		const Byte* getDataVs() const;
		const Byte* getDataGs() const;
		const Byte* getDataPs() const;
		unsigned getSizeVs() const;
		unsigned getSizeGs() const;
		unsigned getSizePs() const;

	private:
		void create(
			unsigned sizeVs, unsigned sizeGs, unsigned sizePs);
		void destroy();

		Byte* mDataVs {nullptr};
		Byte* mDataGs {nullptr};
		Byte* mDataPs {nullptr};
		unsigned mSizeVs {0};
		unsigned mSizeGs {0};
		unsigned mSizePs {0};
	};

	inline Byte* MaterialConstBufferData::getDataVs() {
		return mDataVs;
	}

	inline Byte* MaterialConstBufferData::getDataGs() {
		return mDataGs;
	}

	inline Byte* MaterialConstBufferData::getDataPs() {
		return mDataPs;
	}

	inline const Byte* MaterialConstBufferData::getDataVs() const {
		return mDataVs;
	}

	inline const Byte* MaterialConstBufferData::getDataGs() const {
		return mDataGs;
	}

	inline const Byte* MaterialConstBufferData::getDataPs() const {
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