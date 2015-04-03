#include <iostream>
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../Clair/include/clair/vertexLayout.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

HRESULT compileShader(const std::string& sourceCode, const std::string& target,
					  const std::string& entryPoint, ID3DBlob** blob);

void writeToFile(const std::string& filename);
void writeVertexLayout(FILE* file);


ID3DBlob* vs {nullptr};
ID3DBlob* ps {nullptr};
Clair::VertexLayout vertexLayout {};

int main(int , char* []) {
	//if (argc < 1) return -1;
	//std::cout << "Converting " << argv[1] << std::endl;
	//std::ifstream file(argv[1]);
	std::ifstream file("default.hlsl");
	if (file.fail()) {
		return -1;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	const std::string source = buffer.str();

	HRESULT hr {0};

	// VERTEX SHADER
	hr = compileShader(source, "vs_5_0", "vsMain", &vs);
	if (FAILED(hr)) {
		std::printf("FAILED!\n");
		getchar();
		return -1;
	}
	ID3D11ShaderReflection* pVertexShaderReflection {nullptr};
	if (FAILED(D3DReflect(vs->GetBufferPointer(),
						  vs->GetBufferSize(),
						  IID_ID3D11ShaderReflection,
						  (void**)&pVertexShaderReflection))){
		std::printf("FAILED!\n");
		getchar();
		return -1;
	}
	D3D11_SHADER_DESC shaderDesc {};
	pVertexShaderReflection->GetDesc(&shaderDesc);
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i {0}; i < shaderDesc.InputParameters; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc {};
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);
		vertexLayout.push_back({paramDesc.SemanticName,
								Clair::VertexAttribute::Format::FLOAT3});
	}

	// PIXEL SHADER
	hr = compileShader(source, "ps_5_0", "psMain", &ps);
	if (FAILED(hr)) {
		std::printf("FAILED!\n");
		getchar();
		return -1;
	}
	
	writeToFile("D:/School/Specialization/ClairRenderer/Sample/data/sub.csm");

	// Clean up
	vs->Release();
	ps->Release();
	return 0;
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

void writeToFile(const std::string& filename) {
	FILE* outputFile;
	fopen_s(&outputFile, filename.c_str(), "wb");
	writeVertexLayout(outputFile);
	fclose(outputFile); return;
	//const size_t vsSize = vs->GetBufferSize();
	////fwrite(&vsSize, sizeof(size_t), 1, outputFile);
	//fwrite(vs->GetBufferPointer(), sizeof(char), vsSize, outputFile);
	////const size_t psSize = ps->GetBufferSize();
	////fwrite(&psSize, sizeof(size_t), 1, outputFile);
	////fwrite(ps->GetBufferPointer(), sizeof(char), psSize, outputFile);
	//fclose(outputFile);
}

void writeVertexLayout(FILE* file) {
	const size_t layoutSize {vertexLayout.size()};
	fwrite(&layoutSize, sizeof(size_t), 1, file);
	for (size_t i {0}; i < layoutSize; ++i) {
		const auto element = vertexLayout[i];
		const size_t strSize {element.name.length()};
		fwrite(&strSize, sizeof(size_t), 1, file);
		const char* str {element.name.c_str()};
		fwrite(str, sizeof(char), strSize, file);
		const int format {static_cast<int>(element.format)};
		fwrite(&format, sizeof(int), 1, file);
	}
}