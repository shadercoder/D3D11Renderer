#pragma once

namespace Clair {
	class MaterialConstBufferData {
	public:
		~MaterialConstBufferData();
		char* dataVs {nullptr};
		char* dataGs {nullptr};
		char* dataPs {nullptr};
		size_t sizeVs {0};
		size_t sizeGs {0};
		size_t sizePs {0};
	};

	inline MaterialConstBufferData::~MaterialConstBufferData() {
		if (dataVs) delete dataVs;
		if (dataGs) delete dataGs;
		if (dataPs) delete dataPs;
	}
}