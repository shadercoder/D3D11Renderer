#include "ConstBufferDesc.h"
#include <iostream>
#include <string>
#include <sstream>

void ConstBufferDesc::addVariable(const D3D11_SHADER_VARIABLE_DESC& varDesc,
								  const D3D11_SHADER_TYPE_DESC& typeDesc,
								  ID3D11ShaderReflectionType* /*type*/) {
	mVarNames.push_back(varDesc.Name);
	const std::string typeName {typeDesc.Name};
	mTypeNames.push_back(typeName);
	mSizes.push_back(varDesc.Size);
	mNumElements.push_back(typeDesc.Elements);
	// STRUCTS MAYBE NOT POSSIBLE BECAUSE D3D DOESN'T REFLECT THEIR MEMBERS
	// FOR SOME REASON...
	// Only supports arrays if the elements are structs or 16-byte-aligned
	if (typeDesc.Class != D3D_SVC_STRUCT) {
		if (typeDesc.Elements != 0) {
			if (typeName[typeName.length() - 1] != '4') {
				std::cout <<
					"ERROR: Only 16-byte-aligned array elements supported\n";
				isValid = false;
			}
		}
	} else { // is a struct
		//auto newCb = new ConstBufferDesc{};
		//mStructs[typeName] = newCb;
		//for (UINT i_mem {0}; i_mem < typeDesc.Members; ++i_mem) {
		//	D3D11_SHADER_TYPE_DESC subTypeDesc;
		//	/*if (FAILED(*/
		//	type->GetMemberTypeByIndex(i_mem)->GetDesc(&subTypeDesc);
		//	type->GetInterfaceByIndex(0)->
		//	int a {};
		//	++a;
		//	auto var = ->GetVariableByIndex(i_mem);
		//	D3D11_SHADER_VARIABLE_DESC varDesc;
		//	if (FAILED(var->GetDesc(&varDesc))) {
		//		return MaterialToolError::REFLECT;
		//	}
		//	D3D11_SHADER_TYPE_DESC typeDesc;
		//	if (FAILED(var->GetType()->GetDesc(&typeDesc))) {
		//		return MaterialToolError::REFLECT;
		//	}
		//	outCBuffer.addVariable(varDesc, typeDesc);
		//}
		//newCb->addVariable()
	}
}

std::string ConstBufferDesc::writeToString(const std::string& name,
								  const std::string& postfix) {
	if (!isValid) return {};
	std::stringstream stringStream {};
	stringStream << "\nclass Cb_" << name.c_str() << "_" << postfix.c_str() << " {\n";
	stringStream << "public:\n";
	const size_t numElements {mVarNames.size()};
	unsigned currOffset {0};
	unsigned paddingId {0};
	//unsigned lastDifference = 0;
	for (size_t i {0}; i < numElements; ++i) {
		// write var to stringStream
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
		std::string arrayPostfix {};
		if (mNumElements[i] > 0) {
			arrayPostfix += '[' + std::to_string(mNumElements[i]) + ']';
		}
		stringStream << "\t" << typeName << ' '
			 << mVarNames[i] << arrayPostfix << ";\n";

		// add necessary padding
		currOffset += mSizes[i];
		unsigned room {next16ByteBound(currOffset) - currOffset};
		unsigned nextSize {0};
		if (i + 1 < numElements) {
			nextSize = mSizes[i + 1];
		}
		if (room != 0 && (nextSize == 0 || (nextSize > room))) {
			stringStream << "\tfloat __padding" << paddingId++ << "__["
				 << room / 4 << "];\n";
			currOffset = next16ByteBound(currOffset);
		}
		size = next16ByteBound(currOffset);
	}
	stringStream << "};\n";
	return stringStream.str();
}

void ConstBufferDesc::cleanup() {
	for (const auto& it : mStructs) {
		delete it.second;
	}
}

unsigned ConstBufferDesc::next16ByteBound(const unsigned bytes) {
	unsigned nextBound = 0;
	while (nextBound < bytes) {
		nextBound += 16;
	}
	return nextBound;
}