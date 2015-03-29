#include <Windows.h>
#include "clair/renderer.h"
#include <stdio.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "clair/scene.h"
#include "clair/object.h"
#include <vector>
#include <DirectXMath.h>
#include "clair/matrix.h"
#include "clair/vertexBuffer.h"

using namespace DirectX;
using namespace Clair;

#pragma comment(lib, "d3d11.lib")

ID3D11Device* d3dDevice = nullptr;
ID3D11DeviceContext* d3dDeviceContext = nullptr;
IDXGISwapChain* swapChain = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11DepthStencilView* depthStencilView = nullptr;
ID3D11Texture2D* backBuffer = nullptr;
ID3D11Texture2D* depthStencilBuffer = nullptr;
ID3D11RasterizerState* rasterizerState = nullptr;
std::string dataPath;
std::vector<Clair::Scene*> scenes;

ID3D11InputLayout* inputLayout = nullptr;
ID3D11Buffer* constantBuffer = nullptr;
ID3D11SamplerState* samplerState = nullptr;
ID3D11Texture2D* texture = nullptr;
ID3D11ShaderResourceView* shaderResView = nullptr;

namespace Clair {
	class InputLayout {
	public:
		ID3D11InputLayout* inputLayout = nullptr;
		unsigned stride = 0;
	};
	class Mesh {
	public:
		InputLayout* inputLayout = nullptr;
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		unsigned indexBufferSize = 0;
	};
	class VertexShader {
	public:
		char* byteCode;
		size_t byteCodeSize;
		ID3D11VertexShader* shader;
	};
	class PixelShader {
	public:
		char* byteCode;
		size_t byteCodeSize;
		ID3D11PixelShader* shader;
	};
}
std::vector<VertexShader*> vertexShaders;
std::vector<PixelShader*> pixelShaders;
std::vector<InputLayout*> inputLayouts;
std::vector<Mesh*> meshes;

struct ConstantBuffer {
	Clair::Matrix world;
	XMMATRIX view;
	XMMATRIX projection;
};

template<typename T>
inline static void releaseComObject(T& comObject) {
	if (comObject != NULL) {
		comObject->Release();
		comObject = NULL;
	}
}

bool Clair::Renderer::initialize(HWND hwnd, const std::string& clairDataPath) {
	UINT createDeviceFlags = 0;

#ifndef NDEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	const UINT clientWidth = clientRect.right - clientRect.left;
	const UINT clientHeight = clientRect.bottom - clientRect.top;

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferDesc.Width = clientWidth;
	swapDesc.BufferDesc.Height = clientHeight;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.SampleDesc.Count = 8;
	swapDesc.SampleDesc.Quality = 8;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = hwnd;
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT result;
	D3D_FEATURE_LEVEL featureLevel;
	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
										   nullptr, createDeviceFlags,
										   featureLevels, numFeatureLevels,
										   D3D11_SDK_VERSION, &swapDesc,
										   &swapChain, &d3dDevice,
										   &featureLevel, &d3dDeviceContext);

	if (FAILED(result)) return false;

	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
								  reinterpret_cast<LPVOID*>(&backBuffer));
	if (FAILED(result)) return false;

	result = d3dDevice->CreateRenderTargetView(backBuffer, nullptr,
											   &renderTargetView);
	releaseComObject(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilBufferDesc.Width = clientWidth;
	depthStencilBufferDesc.Height = clientHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 8;
	depthStencilBufferDesc.SampleDesc.Quality = 8;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr,
							   &depthStencilBuffer);
	if (FAILED(result)) return false;

	result = d3dDevice->CreateDepthStencilView(depthStencilBuffer, nullptr,
											   &depthStencilView);
	if (FAILED(result)) return false;

	d3dDeviceContext->OMSetRenderTargets(1, &renderTargetView,
										 depthStencilView);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;

	result = d3dDevice->CreateRasterizerState(&rasterizerDesc,
											  &rasterizerState);
	if (FAILED(result)) return false;

	d3dDeviceContext->RSSetState(rasterizerState);

	D3D11_VIEWPORT viewport = {0};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(clientWidth);
	viewport.Height = static_cast<float>(clientHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	d3dDeviceContext->RSSetViewports(1, &viewport);

	// initialize data path
	dataPath = clairDataPath;
	const auto p = clairDataPath.find_last_not_of("/");
	if (p != std::string::npos)
		dataPath.erase(p + 1);
	dataPath.append("/");
	printf(("Clair initialized.\n\t> Data path: " + dataPath + '\n').c_str());

	d3dDeviceContext->IASetPrimitiveTopology(
							D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = 0;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constInitData;
	ZeroMemory(&constInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	unsigned testInitData[] = { 0, 1, 2, 3, 4 };
	constInitData.pSysMem = testInitData;

	result = d3dDevice->CreateBuffer(&constBufferDesc, &constInitData,
									 &constantBuffer);
	if (FAILED(result)) return false;

	// create texture
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = d3dDevice->CreateSamplerState(&samplerDesc, &samplerState);
	if (FAILED(result)) return false;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = 256;
	texDesc.Height = 256;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	float* texData = new float[256 * 256 * 4]();
	for (unsigned y = 0; y < 256; ++y) {
		for (unsigned x = 0; x < 256; ++x) {
			texData[(x + y * 256) * 4 + 0] = static_cast<float>(x) / 255.0f;
			texData[(x + y * 256) * 4 + 1] = static_cast<float>(y) / 255.0f;
			texData[(x + y * 256) * 4 + 2] = static_cast<float>(rand()) /
											 static_cast<float>(RAND_MAX);
			texData[(x + y * 256) * 4 + 3] = 1.0f;
		}
	}

	D3D11_SUBRESOURCE_DATA texInitData;
	ZeroMemory(&texInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitData.pSysMem = texData;
	texInitData.SysMemPitch = sizeof(float) * 256 * 4;

	result = d3dDevice->CreateTexture2D(&texDesc, &texInitData , &texture);
	if (FAILED(result)) {
		delete[] texData;
		return false;
	}
	result = d3dDevice->CreateShaderResourceView(texture, nullptr,
												 &shaderResView);
	if (FAILED(result)) {
		delete[] texData;
		return false;
	}
	delete[] texData;

	return true;
}

void Clair::Renderer::terminate() {
	for (const auto& it : scenes) {
		delete it;
	}
	if (swapChain) swapChain->SetFullscreenState(FALSE, NULL);
	releaseComObject(shaderResView);
	releaseComObject(texture);
	releaseComObject(samplerState);
	releaseComObject(constantBuffer);
	releaseComObject(inputLayout);
	for (const auto& it : vertexShaders) {
		releaseComObject(it->shader);
		delete it;
	}
	for (const auto& it : pixelShaders) {
		releaseComObject(it->shader);
		delete it;
	}
	for (const auto& it : meshes) {
		releaseComObject(it->vertexBuffer);
		releaseComObject(it->indexBuffer);
		delete it;
	}
	for (const auto& it : inputLayouts) {
		releaseComObject(it->inputLayout);
		delete it;
	}
	releaseComObject(rasterizerState);
	releaseComObject(depthStencilBuffer);
	releaseComObject(depthStencilView);
	releaseComObject(renderTargetView);
	releaseComObject(swapChain);
	releaseComObject(d3dDeviceContext);
	releaseComObject(d3dDevice);
	printf("Clair terminated.\n");
}
float col[] = { 0.1f, 0.2f, 0.3f, 1.0f };
float viewWidth = 640.0f;
float viewHeight = 480.0f;

void Clair::Renderer::clear() {
	d3dDeviceContext->ClearRenderTargetView(renderTargetView, col);
	d3dDeviceContext->ClearDepthStencilView(depthStencilView,
										D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,
										1.0f, 0);
}

void Clair::Renderer::finalizeFrame() {
	swapChain->Present(0, 0);
}

void Clair::Renderer::setViewport(const float x, const float y,
								  const float width, const float height) {
	d3dDeviceContext->OMSetRenderTargets(0, 0, 0);

	// new render target view
	releaseComObject(renderTargetView);
	HRESULT result = 0;
	result = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(result)) return;
	ID3D11Texture2D* buffer = nullptr;
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
								  reinterpret_cast<LPVOID*>(&buffer));
	if (FAILED(result)) return;
	result = d3dDevice->CreateRenderTargetView(buffer, nullptr,
											   &renderTargetView);
	releaseComObject(buffer);

	// new depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilBufferDesc.Width = static_cast<UINT>(width);
	depthStencilBufferDesc.Height = static_cast<UINT>(height);
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 8;
	depthStencilBufferDesc.SampleDesc.Quality = 8;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	releaseComObject(depthStencilBuffer);
	d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr,
							   &depthStencilBuffer);
	if (FAILED(result)) return;

	releaseComObject(depthStencilView);
	result = d3dDevice->CreateDepthStencilView(depthStencilBuffer, nullptr,
											   &depthStencilView);
	if (FAILED(result)) return;

	d3dDeviceContext->OMSetRenderTargets(1, &renderTargetView,
										 depthStencilView);

	// new viewport
	D3D11_VIEWPORT viewport = {0};
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	d3dDeviceContext->RSSetViewports(1, &viewport);
	viewWidth = width;
	viewHeight = height;
}

void Clair::Renderer::render(Scene* const scene) {
	if (!scene) return;
	static float rot = 0.8f;
	rot += 0.0001f;

	const XMMATRIX world = XMMatrixRotationY(rot);
	const XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(cos(rot) * 2.0f, 1.0f,
										   sin(rot) * 2.0f, 0.0f),
										   XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
										   XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	const XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2,
														 viewWidth / viewHeight,
														 0.1f, 100.0f);

	d3dDeviceContext->VSSetShader(vertexShaders[0]->shader, nullptr, 0);
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	d3dDeviceContext->PSSetShader(pixelShaders[0]->shader, nullptr, 0);
	d3dDeviceContext->PSSetShaderResources(0, 1, &shaderResView);
	d3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
	ConstantBuffer cb;
	cb.view = view;
	cb.projection = projection;

	for (const auto& it : scene->mObjects) {
		const Mesh* const mesh = it->getMesh();
		if (!mesh) continue;
		cb.world = it->getMatrix();
		d3dDeviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb,
											0, 0);
		const UINT stride = mesh->inputLayout->stride;
		const UINT offset = 0;
		d3dDeviceContext->IASetInputLayout(mesh->inputLayout->inputLayout);
		d3dDeviceContext->IASetVertexBuffers(0, 1, &mesh->vertexBuffer,
											 &stride, &offset);
		d3dDeviceContext->IASetIndexBuffer(mesh->indexBuffer,
										   DXGI_FORMAT_R32_UINT, 0);
		d3dDeviceContext->DrawIndexed(mesh->indexBufferSize, 0, 0);
	}
}

Clair::Scene* Clair::Renderer::createScene() {
	Clair::Scene* const newScene = new Scene();
	scenes.push_back(newScene);
	return newScene;
}

Clair::InputLayout* Clair::Renderer::createInputLayout(VertexLayout& desc,
													   VertexShader* const vs) {
	InputLayout* newInputLayout = new InputLayout;
	inputLayouts.push_back(newInputLayout);
	HRESULT result;
	std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;
	unsigned stride = 0;
	for (const auto& it : desc.mElements) {
		auto format = DXGI_FORMAT_R32_FLOAT;
		switch (it.format) {
		case VertexLayout::Element::Format::FLOAT2: stride += sizeof(float) * 2;
			format = DXGI_FORMAT_R32G32_FLOAT; break;
		case VertexLayout::Element::Format::FLOAT3: stride += sizeof(float) * 3;
			format = DXGI_FORMAT_R32G32B32_FLOAT; break;
		case VertexLayout::Element::Format::FLOAT4: stride += sizeof(float) * 4;
			format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		}
		layoutDesc.push_back({it.name.c_str(), 0, format, 0, it.offset,
							  D3D11_INPUT_PER_VERTEX_DATA, 0});
	}
	result = d3dDevice->CreateInputLayout(layoutDesc.data(), layoutDesc.size(),
										  vs->byteCode, vs->byteCodeSize,
										  &newInputLayout->inputLayout);
	if (FAILED(result)) return nullptr;
	newInputLayout->stride = stride;
	
	return newInputLayout;
}

Mesh* Renderer::createMesh(MeshDesc& desc) {
	Mesh* newMesh = new Mesh;
	meshes.push_back(newMesh);
	newMesh->inputLayout = desc.inputLayout;
	newMesh->indexBufferSize = desc.numIndices;
	HRESULT result;
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.ByteWidth = desc.vertexDataSize;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResData;
	ZeroMemory(&subResData, sizeof(D3D11_SUBRESOURCE_DATA));
	subResData.pSysMem = desc.vertexData;

	result = d3dDevice->CreateBuffer(&vertexBufferDesc, &subResData,
									 &newMesh->vertexBuffer);
	if (FAILED(result)) return nullptr;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.ByteWidth = desc.numIndices * sizeof(unsigned);
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexInitData;
	ZeroMemory(&indexInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexInitData.pSysMem = desc.indexData;

	result = d3dDevice->CreateBuffer(&indexBufferDesc, &indexInitData,
									 &newMesh->indexBuffer);
	if (FAILED(result)) return nullptr;
	return newMesh;
}

void Clair::Renderer::createMaterial(char* data, VertexShader*& vs,
									 PixelShader*& ps) {
	const size_t vsSize = *reinterpret_cast<size_t*>(data);
	data += sizeof(size_t);
	vs = createVertexShader(data, vsSize);
	data += vsSize;
	const size_t psSize = *reinterpret_cast<size_t*>(data);
	data += sizeof(size_t);
	ps = createPixelShader(data, psSize);
}

Clair::VertexShader* Clair::Renderer::createVertexShader(char* byteCode,
														 size_t size) {
	HRESULT result;
	ID3D11VertexShader* vertexShader = nullptr;
	result = d3dDevice->CreateVertexShader(byteCode, size, nullptr,
										   &vertexShader);
	if (FAILED(result)) {
		return nullptr;
	}
	VertexShader* newShader = new VertexShader{byteCode, size, vertexShader};
	vertexShaders.push_back(newShader);
	return newShader;
}

Clair::PixelShader* Clair::Renderer::createPixelShader(char* byteCode,
													   size_t size) {
	HRESULT result;
	ID3D11PixelShader* pixelShader = nullptr;
	result = d3dDevice->CreatePixelShader(byteCode, size, nullptr,
										  &pixelShader);
	if (FAILED(result)) {
		return nullptr;
	}
	PixelShader* newShader = new PixelShader{byteCode, size, pixelShader};
	pixelShaders.push_back(newShader);
	return newShader;
}

Clair::Matrix bla;
void Clair::Renderer::setCameraMatrix(const Clair::Matrix& m) {
	bla = m;
	return;
}
