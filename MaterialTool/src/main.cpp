#include <iostream>
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

HRESULT compileShader(const std::string& sourceCode, const std::string& target,
					  const std::string& entryPoint, ID3DBlob** blob);

int main(int argc, char* argv[]) {
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

	HRESULT hr = 0;
	ID3DBlob* vs = nullptr;
	ID3DBlob* ps = nullptr;
	hr = compileShader(source, "vs_5_0", "vsMain", &vs);
	if (FAILED(hr)) {
		std::printf("FAILED!\n");
		getchar();
		return -1;
	}
	hr = compileShader(source, "ps_5_0", "psMain", &ps);
	if (FAILED(hr)) {
		std::printf("FAILED!\n");
		getchar();
		return -1;
	}
	FILE* outputFile;
	fopen_s(&outputFile, "test.csm", "wb");
	const SIZE_T vsSize = vs->GetBufferSize();
	//fwrite(&vsSize, sizeof(SIZE_T), 1, outputFile);
	fwrite(vs->GetBufferPointer(), sizeof(char), vsSize, outputFile);
	//const SIZE_T psSize = ps->GetBufferSize();
	//fwrite(&psSize, sizeof(SIZE_T), 1, outputFile);
	//fwrite(ps->GetBufferPointer(), sizeof(char), psSize, outputFile);
	fclose(outputFile);

	// Clean up
	vs->Release();
	ps->Release();
	return 0;
}

HRESULT compileShader(const std::string& sourceCode, const std::string& target,
					  const std::string& entryPoint, ID3DBlob** blob) {
	HRESULT hr = 0;
	*blob = nullptr;

	UINT flags = 0;//D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3DCompile(sourceCode.c_str(), sourceCode.length(), nullptr, nullptr,
					nullptr, entryPoint.c_str(), target.c_str(), 0, 0,
					&shaderBlob, &errorBlob);
	//hr = D3DCompileFromFile(L"default.hlsl", NULL,
	//						D3D_COMPILE_STANDARD_FILE_INCLUDE,
	//						entryPoint.c_str(), target.c_str(), flags,
	//						0, &shaderBlob, &errorBlob );
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

	 // Reflect shader info
	ID3D11ShaderReflection* pVertexShaderReflection = NULL;
	if (FAILED(D3DReflect(shaderBlob->GetBufferPointer(),
						  shaderBlob->GetBufferSize(),
						  IID_ID3D11ShaderReflection,
						  (void**)&pVertexShaderReflection))){
		return S_FALSE;
	}
 
	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc( &shaderDesc );
 
	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);
		int a = 5;
		a++;
	}
	return hr;
}