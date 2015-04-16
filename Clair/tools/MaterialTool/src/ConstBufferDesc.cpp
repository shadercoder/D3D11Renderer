#include "ConstBufferDesc.h"

void ConstBufferDesc::addVariable(const D3D11_SHADER_VARIABLE_DESC& varDesc,
								  const D3D11_SHADER_TYPE_DESC& typeDesc) {
	mVarNames.push_back(varDesc.Name);
	mTypeNames.push_back(typeDesc.Name);
	mSizes.push_back(varDesc.Size);
}

void ConstBufferDesc::writeToFile(std::ofstream& file, const std::string& name,
								  const std::string& postfix) {
	if (!isValid) return;
	file << "\nclass Cb_" << name.c_str() << "_" << postfix.c_str() << " {\n";
	file << "public:\n";
	const size_t numElements {mVarNames.size()};
	unsigned currOffset {0};
	unsigned paddingId {0};
	//unsigned lastDifference = 0;
	for (size_t i {0}; i < numElements; ++i) {
		// write var to file
		std::string hlslName {mTypeNames[i]};
		std::string typeName {hlslName}; 
		if (hlslName[hlslName.length() - 1] == '1') {
			hlslName = hlslName.substr(0, hlslName.length() - 1);
		}
		if (hlslName == "float") typeName = "float";
		else if (hlslName == "int") typeName = "int32_t";
		else if (hlslName == "bool") typeName = "int32_t";
		else {
			typeName[0] = static_cast<char>(toupper(typeName[0]));
			typeName = "Clair::" + typeName;
		}
		file << "\t" << typeName.c_str() << ' '
			 << mVarNames[i].c_str() << ";\n";

		// add necessary padding
		currOffset += mSizes[i];
		unsigned room {next16ByteBound(currOffset) - currOffset};
		unsigned nextSize {0};
		if (i + 1 < numElements) {
			nextSize = mSizes[i + 1];
		}
		if (room != 0 && (nextSize == 0 || (nextSize > room))) {
			file << "\tfloat __padding" << paddingId++ << "__["
				 << room / 4 << "];\n";
			currOffset = next16ByteBound(currOffset);
		}
		size = next16ByteBound(currOffset);
	}
	file << "};\n";
}

unsigned ConstBufferDesc::next16ByteBound(const unsigned bytes) {
	unsigned nextBound = 0;
	while (nextBound < bytes) {
		nextBound += 16;
	}
	return nextBound;
}