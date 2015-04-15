#pragma once
#include <d3d11shader.h>
#include <vector>
#include <fstream>

class ConstBufferDesc {
public:
	void addVariable(const D3D11_SHADER_VARIABLE_DESC& varDesc,
					 const D3D11_SHADER_TYPE_DESC& typeDesc);

	void writeToFile(std::ofstream& file, const std::string& name,
					 const std::string& postfix);

	bool isValid {false};
	std::vector<std::string> mVarNames {};
	std::vector<std::string> mTypeNames {};
};