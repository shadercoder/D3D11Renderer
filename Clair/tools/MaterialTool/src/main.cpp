#include <iostream>
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../../Clair/include/clair/vertexLayout.h"
#include "../../../Clair/src/serialization.h"
#include "ErrorCodes.h"
#include "../../common/CommandLineUtils.h"
#include "ConstBufferDesc.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

static HRESULT compileShader(const std::string& sourceCode,
							 const std::string& target,
							 const std::string& entryPoint,
							 ID3DBlob** blob);

static bool writeToFile(const std::string& filename);
static int reflectVertexLayout(ID3DBlob* vs);
static int reflectConstBuffer(ID3DBlob* shader, ConstBufferDesc& outCBuffer);

static ID3DBlob* gVs {nullptr};
static ID3DBlob* gPs {nullptr};
static Clair::VertexLayout gVertexLayout {};
static std::string gInFile {
	"../../../../samples/common/rawdata/materials/default.hlsl"};
static std::string gOutFile {
	"../../../../samples/common/data/materials/default.cmat"};
static std::string gOutHeader {
	"../../../../samples/common/data/materials/default.h"};
static bool gSilentMode {false};
static std::ofstream gOutHeaderFile {};

//int main(int , char* []) {
int main(int argc, char* argv[]) {
	// Get paths from command arguments (or hardcoded values for debugging)
	if (argc < 3) {
		return MaterialToolError::ARGS;
	}
	gInFile = argv[1];
	gOutFile = argv[2];
	gOutHeader = gOutFile.substr(0, gOutFile.find_last_of(".") + 1) + 'h';
	const auto commands = CommandLineUtils::getCommands(argc - 2, argv + 2);
	for (const char c : commands) {
		if		(c == 's') gSilentMode = true;
	}

	if (!gSilentMode) {
		//std::cout << "Converting " << argv[1] << "\nto " << argv[2] << '\n';
	}

	std::ifstream file(gInFile);
	if (file.fail()) {
		return MaterialToolError::READ;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	const std::string source = buffer.str();

	HRESULT hr {0};

	gOutHeaderFile.open(gOutHeader);
	if (!gOutHeaderFile.is_open()) {
		return MaterialToolError::WRITE;
	}

	// VERTEX SHADER
	hr = compileShader(source, "vs_5_0", "vsMain", &gVs);
	if (FAILED(hr)) {
		if (!gSilentMode) {
			std::printf("FAILED!\n");
		}
		//getchar();
		return MaterialToolError::VS;
	}
	int result = reflectVertexLayout(gVs);
	if (result != MaterialToolError::SUCCESS) {
		return result;
	}
	ConstBufferDesc vsCBufferDesc;
	result = reflectConstBuffer(gVs, vsCBufferDesc);
	if (result != MaterialToolError::SUCCESS) {
		return result;
	}
	gOutHeaderFile << "#pragma once\n\n";
	vsCBufferDesc.writeToFile(gOutHeaderFile, "Default", "Vs");

	// PIXEL SHADER
	hr = compileShader(source, "ps_5_0", "psMain", &gPs);
	if (FAILED(hr)) {
		if (!gSilentMode) {
			std::printf("FAILED!\n");
		}
		//getchar();
		return MaterialToolError::PS;
	}
	ConstBufferDesc psCBufferDesc;
	result = reflectConstBuffer(gPs, psCBufferDesc);
	if (result != MaterialToolError::SUCCESS) {
		return result;
	}
	psCBufferDesc.writeToFile(gOutHeaderFile, "Default", "Ps");

	// finalize and clean up
	if (!writeToFile(gOutFile)) {
		return MaterialToolError::WRITE;
	}
	gOutHeaderFile.close();
	gVs->Release();
	gPs->Release();
	return MaterialToolError::SUCCESS;
}

HRESULT compileShader(const std::string& sourceCode, const std::string& target,
					  const std::string& entryPoint, ID3DBlob** blob) {
	HRESULT hr {0};
	*blob = nullptr;

	const UINT flags {0};//D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG};

	ID3DBlob* shaderBlob {nullptr};
	ID3DBlob* errorBlob {nullptr};
	hr = D3DCompile(sourceCode.c_str(), sourceCode.length(), nullptr, nullptr,
					nullptr, entryPoint.c_str(), target.c_str(), flags, 0,
					&shaderBlob, &errorBlob);
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
			errorBlob->Release();
		}
		if (shaderBlob)
		   shaderBlob->Release();
		return hr;
	}    
	*blob = shaderBlob;
	return hr;
}

bool writeToFile(const std::string& filename) {
	FILE* outputFile;
	if (fopen_s(&outputFile, filename.c_str(), "wb") != 0) {
		return false;
	}
	Clair::Serialization::writeVertexLayoutToFile(outputFile, gVertexLayout);
	const size_t vsSize = gVs->GetBufferSize();
	fwrite(&vsSize, sizeof(size_t), 1, outputFile);
	fwrite(gVs->GetBufferPointer(), sizeof(char), vsSize, outputFile);
	const size_t psSize = gPs->GetBufferSize();
	fwrite(&psSize, sizeof(size_t), 1, outputFile);
	fwrite(gPs->GetBufferPointer(), sizeof(char), psSize, outputFile);
	fclose(outputFile);
	return true;
}

int reflectVertexLayout(ID3DBlob* const vs) {
	ID3D11ShaderReflection* shaderReflection {nullptr};
	if (FAILED(D3DReflect(vs->GetBufferPointer(),
						  vs->GetBufferSize(),
						  IID_ID3D11ShaderReflection,
						  (void**)&shaderReflection))){
		return MaterialToolError::REFLECT;
	}
	D3D11_SHADER_DESC shaderDesc {};
	shaderReflection->GetDesc(&shaderDesc);
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i {0}; i < shaderDesc.InputParameters; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc {};
		shaderReflection->GetInputParameterDesc(i, &paramDesc);
		gVertexLayout.push_back({paramDesc.SemanticName,
								Clair::VertexAttribute::Format::FLOAT3});
	}
	return MaterialToolError::SUCCESS;
}

int reflectConstBuffer(ID3DBlob* const shader, ConstBufferDesc& outCBuffer) {
	ID3D11ShaderReflection* shaderReflection {nullptr};
	if (FAILED(D3DReflect(shader->GetBufferPointer(),
						  shader->GetBufferSize(),
						  IID_ID3D11ShaderReflection,
						  (void**)&shaderReflection))){
		return MaterialToolError::REFLECT;
	}
	// Find constant buffer with slot 1
	for (int i {0}; ; ++i) {
		auto constBuffer = shaderReflection->GetConstantBufferByIndex(i);
		if (!constBuffer) {
			break;
		}
		D3D11_SHADER_BUFFER_DESC constBufferDesc {};
		if (FAILED(constBuffer->GetDesc(&constBufferDesc))) {
			break;
		}
		for (int j {0}; ; ++j) {
			HRESULT hres {};
			D3D11_SHADER_INPUT_BIND_DESC inputBindDesc;
			hres = shaderReflection->GetResourceBindingDesc(j, &inputBindDesc);
			if (FAILED(hres)) {
				break;
			}
			if (inputBindDesc.BindPoint == 1
				&& strcmp(constBufferDesc.Name, inputBindDesc.Name) == 0) {
				outCBuffer.isValid = true;
				// found cbuffer in slot 1
				for (UINT iVar {0}; iVar < constBufferDesc.Variables; ++iVar) {
					auto var = constBuffer->GetVariableByIndex(iVar);
					D3D11_SHADER_VARIABLE_DESC varDesc;
					if (FAILED(var->GetDesc(&varDesc))) {
						return MaterialToolError::REFLECT;
					}
					D3D11_SHADER_TYPE_DESC typeDesc;
					if (FAILED(var->GetType()->GetDesc(&typeDesc))) {
						return MaterialToolError::REFLECT;
					}
					outCBuffer.addVariable(varDesc, typeDesc);
				}
			}
		}
	}
	return MaterialToolError::SUCCESS;
}