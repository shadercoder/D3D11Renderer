/* MaterialTool
** Usage:
** MaterialTool.exe (input_file) (output_location) (log_location) (name) [options]
*/
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
#include "IncludedFiles.h"

#include <direct.h>
#define GetCurrentDir _getcwd

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
static std::string gInFileName {
	"../../../../samples/common/rawdata/materials/deferredComposite.hlsl"};
static std::string gOutFileName {
	"../../../../samples/common/data/materials/deferredComposite.cmat"};
static std::string gOutHeaderName {
	"../../../../samples/common/data/materials/deferredComposite.h"};
static std::string gLogFileName {
	"../../../../samples/common/data/log.txt"};
static bool gSilentMode {false};
static std::ofstream gOutHeaderFile {};
static std::string gMaterialName {"material"};
static ConstBufferDesc gVsCBufferDesc;
static ConstBufferDesc gPsCBufferDesc;
static std::ofstream gLogFile {};
static IncludedFiles gIncludedFiles {};
static std::string gWorkinDirectory {'/'};

//int main(int , char* []) {
int main(int argc, char* argv[]) {
	// Get paths from command arguments (or hardcoded values for debugging)
	if (argc < 4) {
		//return MaterialToolError::ARGS;
	}
	else {
		gInFileName = argv[1];
		gOutFileName = std::string(argv[2]) + ".cmat";
		gOutHeaderName = std::string(argv[2]) + ".h";
		gLogFileName = std::string(argv[3]);
		gMaterialName = argv[4];
		const auto commands = CommandLineUtils::getCommands(argc - 4, argv + 4);
		for (const char c : commands) {
			if		(c == 's') gSilentMode = true;
		}
		if (!gSilentMode) {
			std::cout << "Converting " << argv[1] << '\n';
		}
	}
	auto findLastSlashPos = gInFileName.find_last_of("/\\");
	if (findLastSlashPos != std::string::npos) {
		gWorkinDirectory = gInFileName.substr(0, findLastSlashPos) + '/';
	}
	//std::cout << "error: " << gWorkinDirectory << '\n';
	//std::cout << "error: " << gInFileName << '\n';

	gLogFile.open(gLogFileName);
	if (!gLogFile.is_open()) {
		return MaterialToolError::WRITE;
	}

	std::ifstream file(gInFileName);
	if (file.fail()) {
		return MaterialToolError::READ;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	const std::string source = buffer.str();

	// VERTEX SHADER
	if (FAILED(compileShader(source, "vs_5_0", "vsMain", &gVs))) {
		if (!gSilentMode) {
			std::printf("FAILED!\n");
		}
		return MaterialToolError::VS;
	}
	int result = reflectVertexLayout(gVs);
	if (result != MaterialToolError::SUCCESS) {
		return result;
	}
	result = reflectConstBuffer(gVs, gVsCBufferDesc);
	if (result != MaterialToolError::SUCCESS) {
		return result;
	}

	// PIXEL SHADER
	if (FAILED(compileShader(source, "ps_5_0", "psMain", &gPs))) {
		if (!gSilentMode) {
			std::printf("FAILED!\n");
		}
		return MaterialToolError::PS;
	}
	result = reflectConstBuffer(gPs, gPsCBufferDesc);
	if (result != MaterialToolError::SUCCESS) {
		return result;
	}

	// finalize and clean up
	if (!writeToFile(gOutFileName)) {
		return MaterialToolError::WRITE;
	}
	gIncludedFiles.writeToLog(gLogFile);
	gVs->Release();
	gPs->Release();
	gLogFile.close();
	return MaterialToolError::SUCCESS;
}

		//char cCurrentPath[FILENAME_MAX]; 
		//if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
		//	return errno;
		//}
		//printf("Error: The current working directory is %s", cCurrentPath);
class CustomIncludeInterface : public ID3DInclude {
public:
	STDMETHOD(Open)(THIS_ D3D_INCLUDE_TYPE /*IncludeType*/,
		LPCSTR pFileName, LPCVOID /*pParentData*/, LPCVOID *ppData,
		UINT *pBytes) override {
		const std::string filename {gWorkinDirectory + pFileName};
		std::ifstream f(filename);
		if (!f.is_open()) {
			std::cout << "Error: Couldn't open "  << filename << '\n';
			return S_OK;
		}
		gIncludedFiles.addFile(pFileName);
		mString = std::string{
			std::istreambuf_iterator<char>{f},
			std::istreambuf_iterator<char>{}
		};
		*ppData = mString.c_str();
		*pBytes = mString.size();
		return S_OK;
	}

	STDMETHOD(Close)(THIS_ LPCVOID /*pData*/) override {
		return S_OK;
	}

private:
	std::string mString {};
};

HRESULT compileShader(const std::string& sourceCode, const std::string& target,
					  const std::string& entryPoint, ID3DBlob** blob) {
	HRESULT hr {0};
	*blob = nullptr;

	const UINT flags {D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG};

	ID3DBlob* shaderBlob {nullptr};
	ID3DBlob* errorBlob {nullptr};
	CustomIncludeInterface incInterface {};
	hr = D3DCompile(sourceCode.c_str(), sourceCode.length(),
					gInFileName.c_str(), nullptr,
					static_cast<ID3DInclude*>(&incInterface),
					entryPoint.c_str(), target.c_str(), flags, 0,
					&shaderBlob, &errorBlob);
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
			gLogFile << (char*)errorBlob->GetBufferPointer();
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
	// header
	//if (gVsCBufferDesc.isValid || gPsCBufferDesc.isValid) {
		gOutHeaderFile.open(gOutHeaderName);
		if (!gOutHeaderFile.is_open()) {
			return false;
		}
		gOutHeaderFile << "#pragma once\n";
		gOutHeaderFile << gVsCBufferDesc.writeToString(gMaterialName, "Vs");
		gOutHeaderFile << gPsCBufferDesc.writeToString(gMaterialName, "Ps");
		gOutHeaderFile.close();
	//}

	// cmat
	FILE* outputFile;
	if (fopen_s(&outputFile, filename.c_str(), "wb") != 0) {
		return false;
	}
	Clair::Serialization::writeVertexLayoutToFile(outputFile, gVertexLayout);
	fwrite(&gVsCBufferDesc.size, sizeof(unsigned), 1, outputFile);
	fwrite(&gPsCBufferDesc.size, sizeof(unsigned), 1, outputFile);
	const size_t vsSize = gVs->GetBufferSize();
	fwrite(&vsSize, sizeof(size_t), 1, outputFile);
	fwrite(gVs->GetBufferPointer(), sizeof(Byte), vsSize, outputFile);
	const size_t psSize = gPs->GetBufferSize();
	fwrite(&psSize, sizeof(size_t), 1, outputFile);
	fwrite(gPs->GetBufferPointer(), sizeof(Byte), psSize, outputFile);
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
		HRESULT hr {};
		if (FAILED(hr = constBuffer->GetDesc(&constBufferDesc))) {
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
					auto type = var->GetType();
					D3D11_SHADER_TYPE_DESC typeDesc;
					if (FAILED(type->GetDesc(&typeDesc))) {
						return MaterialToolError::REFLECT;
					}
					outCBuffer.addVariable(varDesc, typeDesc, type);
				}
			}
		}
	}
	return MaterialToolError::SUCCESS;
}