#include <Windows.h>
#include "LowLevelRenderer.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Clair/Matrix.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Mesh.h"
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include "Material.h"
#include "Clair/RenderPass.h"

#pragma comment(lib, "d3d11.lib")

using namespace Clair;
using namespace DirectX;

namespace Clair {
	class InputLayout {
	public:
		ID3D11InputLayout* d3dInputLayout {nullptr};
		unsigned stride {0};
	};

	class VertexShader {
	public:
		char* byteCode {nullptr};
		size_t byteCodeSize {0};
		ID3D11VertexShader* d3dShader {nullptr};
	};
	class PixelShader {
	public:
		ID3D11PixelShader* d3dShader {nullptr};
	};
}

namespace {
	ID3D11Device* d3dDevice {nullptr};
	ID3D11DeviceContext* d3dDeviceContext {nullptr};
	IDXGISwapChain* swapChain {nullptr};
	ID3D11RenderTargetView* renderTargetView {nullptr};
	ID3D11DepthStencilView* depthStencilView {nullptr};
	ID3D11Texture2D* backBuffer {nullptr};
	ID3D11Texture2D* depthStencilBuffer {nullptr};
	ID3D11RasterizerState* rasterizerState {nullptr};

	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;
	ID3D11SamplerState* samplerState = nullptr;
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* shaderResView = nullptr;

	std::vector<InputLayout*> inputLayouts;

	Matrix cameraViewMat {};
	RenderPass gRenderPass {};
}

struct ConstantBuffer {
	Clair::Matrix world;
	Clair::Matrix view;
	XMMATRIX projection;
};

template<typename T>
inline static void releaseComObject(T& comObject) {
	if (comObject != NULL) {
		comObject->Release();
		comObject = NULL;
	}
}

bool LowLevelRenderer::initialize(const HWND hwnd) {
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
	rasterizerDesc.CullMode = D3D11_CULL_BACK;//D3D11_CULL_NONE;
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

void LowLevelRenderer::terminate() {
	if (swapChain) swapChain->SetFullscreenState(FALSE, NULL);
	releaseComObject(shaderResView);
	releaseComObject(texture);
	releaseComObject(samplerState);
	releaseComObject(constantBuffer);
	releaseComObject(inputLayout);
	for (const auto& it : inputLayouts) {
		releaseComObject(it->d3dInputLayout);
		delete it;
	}
	releaseComObject(rasterizerState);
	releaseComObject(depthStencilBuffer);
	releaseComObject(depthStencilView);
	releaseComObject(renderTargetView);
	releaseComObject(swapChain);
	releaseComObject(d3dDeviceContext);
	releaseComObject(d3dDevice);
}

void LowLevelRenderer::clear() {
	const float col[] {0.2f, 0.4f, 0.6f, 1.0f};
	d3dDeviceContext->ClearRenderTargetView(renderTargetView, col);
	d3dDeviceContext->ClearDepthStencilView(
										depthStencilView,
										D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,
										1.0f, 0);
}

void LowLevelRenderer::finalizeFrame() {
	swapChain->Present(0, 0);
}

static float viewWidth = 640.0f;
static float viewHeight = 480.0f;

void LowLevelRenderer::setViewport(const int x, const int y,
								   const int width, const int height) {
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
	viewport.TopLeftX = static_cast<float>(x);
	viewport.TopLeftY = static_cast<float>(y);
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	d3dDeviceContext->RSSetViewports(1, &viewport);
	viewWidth = static_cast<float>(width);
	viewHeight = static_cast<float>(height);
}

void LowLevelRenderer::render(Scene* const scene) {
	const XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2,
														 viewWidth / viewHeight,
														 0.1f, 100.0f);

	d3dDeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	d3dDeviceContext->PSSetShaderResources(0, 1, &shaderResView);
	d3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
	ConstantBuffer cb;
	cb.view = cameraViewMat;
	cb.projection = projection;

	for (const auto& it : scene->mObjects) {
		const Mesh* const mesh {it->getMesh()};
		if (!mesh) continue;
		auto material = it->getMaterial(gRenderPass);
		if (!material) continue;
		auto vs = material->vertexShader->d3dShader;
		auto ps = material->pixelShader->d3dShader;
		d3dDeviceContext->VSSetShader(vs, nullptr, 0);
		d3dDeviceContext->PSSetShader(ps, nullptr, 0);
		cb.world = it->getMatrix();
		d3dDeviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb,
											0, 0);
		const UINT stride {it->getInputLayout()->stride};
		const UINT offset {0};
		d3dDeviceContext->IASetInputLayout(
			it->getInputLayout()->d3dInputLayout);
		d3dDeviceContext->IASetVertexBuffers(0, 1,
											 &mesh->vertexBuffer->d3dBuffer,
											 &stride, &offset);
		d3dDeviceContext->IASetIndexBuffer(mesh->indexBuffer->d3dBuffer,
										   DXGI_FORMAT_R32_UINT, 0);
		d3dDeviceContext->DrawIndexed(mesh->indexBufferSize, 0, 0);
	}
}

void LowLevelRenderer::setCameraMatrix(const Matrix& m) {
	cameraViewMat = m;
}

void LowLevelRenderer::setRenderPass(const RenderPass pass) {
	gRenderPass = pass;
}

VertexBuffer* LowLevelRenderer::createVertexBuffer(char* const bufferData,
												   const unsigned bufferSize) {
	VertexBuffer* const vertexBuffer {new VertexBuffer};
	HRESULT result {};
	D3D11_BUFFER_DESC vertexBufferDesc {};
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.ByteWidth = bufferSize;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResData {};
	ZeroMemory(&subResData, sizeof(D3D11_SUBRESOURCE_DATA));
	subResData.pSysMem = bufferData;

	result = d3dDevice->CreateBuffer(&vertexBufferDesc, &subResData,
									 &vertexBuffer->d3dBuffer);
	if (FAILED(result)) {
		return nullptr;
	}
	return vertexBuffer;
}

IndexBuffer* LowLevelRenderer::createIndexBuffer(unsigned* const bufferData,
												 const unsigned bufferSize) {
	IndexBuffer* const indexBuffer {new IndexBuffer};
	HRESULT result {};
	D3D11_BUFFER_DESC indexBufferDesc {};
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.ByteWidth = bufferSize;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexInitData {};
	ZeroMemory(&indexInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexInitData.pSysMem = bufferData;

	result = d3dDevice->CreateBuffer(&indexBufferDesc, &indexInitData,
									 &indexBuffer->d3dBuffer);
	if (FAILED(result)) {
		return nullptr;
	}
	return indexBuffer;
}

InputLayout* LowLevelRenderer::createInputLayout(const VertexLayout&
												 vertexLayout,
												 VertexShader* const
												 vertexShader) {
	InputLayout* const inputLayout {new InputLayout};
	HRESULT result {};
	std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc {};
	unsigned stride {0};
	unsigned offset {0};
	for (const auto& it : vertexLayout) {
		auto format = DXGI_FORMAT_R32_FLOAT;
		offset = stride;
		switch (it.format) {
		case VertexAttribute::Format::FLOAT1: stride += sizeof(float) * 1;
			format = DXGI_FORMAT_R32_FLOAT; break;
		case VertexAttribute::Format::FLOAT2: stride += sizeof(float) * 2;
			format = DXGI_FORMAT_R32G32_FLOAT; break;
		case VertexAttribute::Format::FLOAT3: stride += sizeof(float) * 3;
			format = DXGI_FORMAT_R32G32B32_FLOAT; break;
		case VertexAttribute::Format::FLOAT4: stride += sizeof(float) * 4;
			format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		}
		layoutDesc.push_back({it.name.c_str(), 0, format, 0, offset,
							  D3D11_INPUT_PER_VERTEX_DATA, 0});
	}
	result = d3dDevice->CreateInputLayout(layoutDesc.data(), layoutDesc.size(),
										  vertexShader->byteCode,
										  vertexShader->byteCodeSize,
										  &inputLayout->d3dInputLayout);
	if (FAILED(result)) return nullptr;
	inputLayout->stride = stride;
	return inputLayout;
}

VertexShader* LowLevelRenderer::createVertexShader(char* const vsData,
												   const size_t vsSize) {
	HRESULT result {};
	ID3D11VertexShader* vertexShader {nullptr};
	result = d3dDevice->CreateVertexShader(vsData, vsSize, nullptr,
										   &vertexShader);
	if (FAILED(result)) {
		return nullptr;
	}
	VertexShader* newShader = new VertexShader{};
	newShader->byteCode = vsData;
	newShader->byteCodeSize = vsSize;
	newShader->d3dShader = vertexShader;
	return newShader;
}

PixelShader* LowLevelRenderer::createPixelShader(char* const psData,
												 const size_t psSize) {
	HRESULT result {};
	ID3D11PixelShader* pixelShader {nullptr};
	result = d3dDevice->CreatePixelShader(psData, psSize, nullptr,
										  &pixelShader);
	if (FAILED(result)) {
		return nullptr;
	}
	PixelShader* newShader = new PixelShader{};
	newShader->d3dShader = pixelShader;
	return newShader;
}

void LowLevelRenderer::destroyVertexShader(VertexShader* const vertexShader) {
	releaseComObject(vertexShader->d3dShader);
	delete vertexShader;
}

void LowLevelRenderer::destroyPixelShader(PixelShader* const pixelShader) {
	releaseComObject(pixelShader->d3dShader);
	delete pixelShader;
}

void LowLevelRenderer::destroyVertexBuffer(VertexBuffer* const vertexBuffer) {
	releaseComObject(vertexBuffer->d3dBuffer);
	delete vertexBuffer;
}

void LowLevelRenderer::destroyIndexBuffer(IndexBuffer* const indexBuffer) {
	releaseComObject(indexBuffer->d3dBuffer);
	delete indexBuffer;
}

void LowLevelRenderer::destroyInputLayout(InputLayout* const inputLayout) {
	releaseComObject(inputLayout->d3dInputLayout);
	delete inputLayout;
}