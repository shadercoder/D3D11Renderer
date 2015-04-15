#include "ConstBufferDesc.h"

void ConstBufferDesc::addVariable(const D3D11_SHADER_VARIABLE_DESC& varDesc,
								  const D3D11_SHADER_TYPE_DESC& typeDesc) {
	mVarNames.push_back(varDesc.Name);
	mTypeNames.push_back(typeDesc.Name);
}

void ConstBufferDesc::writeToFile(std::ofstream& file, const std::string& name,
								  const std::string& postfix) {
	if (!isValid) return;
	file << "class Cb_" << name.c_str() << "_" << postfix.c_str() << " {\n";
	file << "public:\n";
	for (size_t i {0}; i < mVarNames.size(); ++i) {
		std::string typeName = mTypeNames[i];
		typeName[0] = static_cast<char>(toupper(typeName[0]));
		file << "\tClair::" << typeName.c_str() << ' '
			 << mVarNames[i].c_str() << ";\n";
	}
	file << "};\n";
}