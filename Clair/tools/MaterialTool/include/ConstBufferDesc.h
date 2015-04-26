#pragma once
#include <d3d11shader.h>
#include <vector>
#include <fstream>
#include <map>

class ConstBufferDesc {
public:
	void addVariable(const D3D11_SHADER_VARIABLE_DESC& varDesc,
					 const D3D11_SHADER_TYPE_DESC& typeDesc,
					 ID3D11ShaderReflectionType* type);

	std::string writeToString(const std::string& name,
					 const std::string& postfix);

	void cleanup();

	bool isValid {false};
	unsigned size {0};

private:
	static unsigned next16ByteBound(unsigned bytes);
	std::vector<std::string> mVarNames {};
	std::vector<std::string> mTypeNames {};
	std::vector<unsigned> mSizes {};
	std::vector<unsigned> mNumElements {};
	std::map<std::string, ConstBufferDesc*> mStructs {};
};