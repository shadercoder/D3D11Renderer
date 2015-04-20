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
#include "Clair/Material.h"
#include "Clair/RenderPass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "ConstantBuffer.h"
#include "Clair/MaterialInstance.h"

#pragma comment(lib, "d3d11.lib")

using namespace Clair;
using namespace DirectX;

namespace {
	ID3D11Device* d3dDevice {nullptr};
	ID3D11DeviceContext* d3dDeviceContext {nullptr};
	IDXGISwapChain* swapChain {nullptr};
	ID3D11RenderTargetView* renderTargetView {nullptr};
	ID3D11DepthStencilView* depthStencilView {nullptr};
	ID3D11Texture2D* backBuffer {nullptr};
	ID3D11Texture2D* depthStencilBuffer {nullptr};
	ID3D11RasterizerState* rasterizerState {nullptr};

	ID3D11SamplerState* samplerState = nullptr;

	Float4x4 cameraView {};
	Float4x4 cameraProjection {};
	Float3 cameraPosition {};
	RenderPass gRenderPass {};

	struct ConstantBufferTemp {
		Float4x4 world;
		Float4x4 view;
		Float4x4 projection;
		Float3 camPos;
		float padding;
	};
	ID3D11Buffer* constantBuffer = nullptr;

	Float3 gQuadVertices[] {
		{0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f}
	};
	unsigned gQuadIndices[] {
		0, 1, 2,
		2, 3, 0
	};
	VertexLayout gQuadVertLayout {};
	VertexBuffer* gQuadVertexBuffer {nullptr};
	IndexBuffer* gQuadIndexBuffer {nullptr};
	InputLayout* gQuadInputLayout {nullptr};
}

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
	constBufferDesc.ByteWidth = sizeof(ConstantBufferTemp);
	constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = 0;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constInitData;
	ZeroMemory(&constInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	const unsigned testInitData[] {0, 1, 2, 3, 4};
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

	// quad
	gQuadVertLayout.push_back(VertexAttribute{"POSITION",
							  VertexAttribute::Format::FLOAT3});
	gQuadVertexBuffer = new VertexBuffer{reinterpret_cast<Byte*>(gQuadVertices),
										 sizeof(Float3) * 4};
	gQuadIndexBuffer = new IndexBuffer{gQuadIndices, sizeof(unsigned) * 6};

	return true;
}

void LowLevelRenderer::terminate() {
	if (swapChain) swapChain->SetFullscreenState(FALSE, NULL);
	if (gQuadInputLayout) delete gQuadInputLayout;
	delete gQuadIndexBuffer;
	delete gQuadVertexBuffer;
	releaseComObject(samplerState);
	releaseComObject(constantBuffer);
	releaseComObject(rasterizerState);
	releaseComObject(depthStencilBuffer);
	releaseComObject(depthStencilView);
	releaseComObject(renderTargetView);
	releaseComObject(swapChain);
	releaseComObject(d3dDeviceContext);
	releaseComObject(d3dDevice);
}

ID3D11Device* LowLevelRenderer::getD3dDevice() {
	return d3dDevice;
}


void LowLevelRenderer::clear(const bool clearCol) {
	if (clearCol) {
		const float col[] {0.2f, 0.4f, 0.6f, 1.0f};
		d3dDeviceContext->ClearRenderTargetView(renderTargetView, col);
	}
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
	d3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

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
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	d3dDeviceContext->PSSetConstantBuffers(0, 1, &constantBuffer);
	d3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
	ConstantBufferTemp cb;
	cb.view = cameraView;
	cb.projection = cameraProjection;
	cb.camPos = cameraPosition;

	//int iteration {0};
	for (const auto& it : scene->mObjects) {
		const Mesh* const mesh {it->getMesh()};
		if (!mesh) continue;
		const auto matInstance = it->getMaterial(gRenderPass);
		const auto material = matInstance->getMaterial();
		if (!material || !material->isValid()) {
			continue;
		}
		const auto vs = material->getVertexShader()->getD3dShader();
		const auto ps = material->getPixelShader()->getD3dShader();
		// material const buffer start
		const auto matCb = material->getConstantBufferPs();
		const auto texMap = matInstance->getTextureMap();
		for (const auto& itTex : texMap) {
			auto const resView = itTex.second->getD3DShaderResourceView();
			d3dDeviceContext->PSSetShaderResources(itTex.first, 1, &resView);
		}
		{//if (iteration++ == 0) {
			if (matCb) {
				if (!matCb->isValid()) continue;
				const auto matCbData = it->getMaterial(gRenderPass)->
										getConstBufferData()->getDataPs();
				const auto matD3d = matCb->getD3dBuffer();
				d3dDeviceContext->UpdateSubresource(matD3d, 0, nullptr, matCbData,
													0, 0);
				d3dDeviceContext->PSSetConstantBuffers(1, 1, &matD3d);
			}
			// material const buffer end
			d3dDeviceContext->VSSetShader(vs, nullptr, 0);
			d3dDeviceContext->PSSetShader(ps, nullptr, 0);
		}
		cb.world = it->getMatrix();
		d3dDeviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb,
											0, 0);
		const UINT stride {it->getInputLayout()->getStride()};
		const UINT offset {0};
		d3dDeviceContext->IASetInputLayout(
			it->getInputLayout()->getD3dInputLayout());
		ID3D11Buffer* const vertexBuffer {
			mesh->getVertexBuffer()->getD3dBuffer()
		};
		d3dDeviceContext->IASetVertexBuffers(0, 1,
											 &vertexBuffer,
											 &stride, &offset);
		d3dDeviceContext->IASetIndexBuffer(
			mesh->getIndexBuffer()->getD3dBuffer(), DXGI_FORMAT_R32_UINT, 0);
		d3dDeviceContext->DrawIndexed(mesh->getIndexBufferSize(), 0, 0);
	}
}

void LowLevelRenderer::setViewMatrix(const Float4x4& view) {
	cameraView = view;
}

void LowLevelRenderer::setProjectionMatrix(const Float4x4& projection) {
	cameraProjection = projection;
}

void LowLevelRenderer::setRenderPass(const RenderPass pass) {
	gRenderPass = pass;
}

void LowLevelRenderer::setCameraPosition(const Float3& position) {
	cameraPosition = position;
}

void LowLevelRenderer::renderScreenQuad(
	const MaterialInstance* const materialInstance) {
	CLAIR_ASSERT(materialInstance, "Material instance should not be null");
	const Material* const mat {materialInstance->getMaterial()};
	if (!gQuadInputLayout || !gQuadInputLayout->isValid()) {
		if (gQuadInputLayout) {
			delete gQuadInputLayout;
		}
		gQuadInputLayout = new InputLayout{gQuadVertLayout,
							mat->getVertexShader()};
		CLAIR_DEBUG_LOG_IF(!gQuadInputLayout->isValid(),
				"Couldn't create the input layout for the full screen quad");
		//return;
	}
	d3dDeviceContext->VSSetShader(
		mat->getVertexShader()->getD3dShader(), nullptr, 0);
	d3dDeviceContext->PSSetShader(
		mat->getPixelShader()->getD3dShader(), nullptr, 0);

	const auto matCbData = materialInstance->getConstBufferData()->getDataPs();
	const auto matD3d = mat->getConstantBufferPs()->getD3dBuffer();
	d3dDeviceContext->UpdateSubresource(matD3d, 0, nullptr, matCbData,
										0, 0);
	d3dDeviceContext->PSSetConstantBuffers(1, 1, &matD3d);

	const UINT stride {sizeof(Float3)};
	const UINT offset {0};
	d3dDeviceContext->IASetInputLayout(gQuadInputLayout->getD3dInputLayout());
	ID3D11Buffer* const vertexBuffer {gQuadVertexBuffer->getD3dBuffer()};
	d3dDeviceContext->IASetVertexBuffers(0, 1,
										 &vertexBuffer,
										 &stride, &offset);
	d3dDeviceContext->IASetIndexBuffer(
		gQuadIndexBuffer->getD3dBuffer(), DXGI_FORMAT_R32_UINT, 0);
	d3dDeviceContext->DrawIndexed(6, 0, 0);
}
