#include <Windows.h>
#include "Clair/Renderer.h"
#include "D3dDevice.h"
#include <DirectXMath.h>
#include "Clair/Matrix.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Clair/Mesh.h"
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
#include "Clair/RenderTargetGroup.h"

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

RenderTargetGroup* Renderer::msDefaultRenderTargetGroup {nullptr};

template<typename T>
inline static void releaseComObject(T& comObject) {
	if (comObject != NULL) {
		comObject->Release();
		comObject = NULL;
	}
}

bool Renderer::initialize(const HWND hwnd) {
	d3dDevice = D3dDevice::getD3dDevice();
	d3dDeviceContext = D3dDevice::getD3dDeviceContext();
	swapChain = D3dDevice::getSwapChain();

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	const UINT clientWidth = clientRect.right - clientRect.left;
	const UINT clientHeight = clientRect.bottom - clientRect.top;

	HRESULT result {};
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
								  reinterpret_cast<LPVOID*>(&backBuffer));
	
	if (FAILED(result)) return false;

	result = d3dDevice->CreateRenderTargetView(backBuffer, nullptr,
											   &renderTargetView);
	releaseComObject(backBuffer);
	if (FAILED(result)) return false;

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilBufferDesc.Width = clientWidth;
	depthStencilBufferDesc.Height = clientHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	result = d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr,
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

	//Texture* defTex = new Texture();
	//defTex->mIsValid = false;
	//defTex->mD3dTexture = backBuffer;
	//defTex->mType = Texture::Type::RENDER_TARGET;
	//Texture* depthTex = new Texture();
	//depthTex->mIsValid = false;
	//depthTex->mD3dTexture = depthStencilBuffer;
	//depthTex->mType = Texture::Type::DEPTH_STENCIL_TARGET;
	//RenderTarget* defRenTarget = new RenderTarget();
	////defRenTarget->initialize(defTex);
	//defRenTarget->mIsValid = false;
	//defRenTarget->mD3dRenderTargetView = renderTargetView;
	//DepthStencilTarget* depthTarget = new DepthStencilTarget();
	////depthTarget->initialize(depthTex);
	//depthTarget->mD3dDepthStencilTargetView = depthStencilView;
	//msDefaultRenderTargetGroup = new RenderTargetGroup(1);
	//msDefaultRenderTargetGroup->setRenderTarget(0, defRenTarget);
	//msDefaultRenderTargetGroup->setDepthStencilTarget(depthTarget);
	//delete msDefaultRenderTargetGroup;
	//delete depthTarget;
	//delete defRenTarget;
	//delete depthTex;
	//delete defTex;
	
	return true;
}

void Renderer::terminate() {
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
	//releaseComObject(swapChain);
	//releaseComObject(d3dDeviceContext);
	//releaseComObject(d3dDevice);
}

void Renderer::clearColor(const Float4& value) {
	d3dDeviceContext->ClearRenderTargetView(renderTargetView, &value[0][0]);
}

void Renderer::clearDepthStencil(const float depthValue,
								 const unsigned char stencilValue) {
	d3dDeviceContext->ClearDepthStencilView(
										depthStencilView,
										D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,
										depthValue, stencilValue);
}


void Renderer::finalizeFrame() {
	swapChain->Present(0, 0);
}

void Renderer::setRenderTargetGroup(const RenderTargetGroup* targets) {
	if (!targets) {
		d3dDeviceContext->OMSetRenderTargets(1, &renderTargetView,
											 depthStencilView);
	} else {
		auto targetArray = targets->getD3dRenderTargetArray();
		CLAIR_ASSERT(targetArray, "Render targets should not be null");
		auto numTargets = targets->getNumRenderTargets();
		d3dDeviceContext->OMSetRenderTargets(numTargets, targetArray,
			targets->getDepthStencilTarget()->mD3dDepthStencilTargetView);
	}
}

static float viewWidth = 640.0f;
static float viewHeight = 480.0f;

void Renderer::setViewport(const int x, const int y,
								   const int width, const int height) {
	d3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	// new render target view
	releaseComObject(renderTargetView);
	HRESULT result {0};
	result = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	CLAIR_ASSERT(!FAILED(result), "Viewport resize error");
	ID3D11Texture2D* buffer {nullptr};
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
								  reinterpret_cast<LPVOID*>(&buffer));
	CLAIR_ASSERT(!FAILED(result), "Viewport resize error");
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
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	releaseComObject(depthStencilBuffer);
	d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr,
							   &depthStencilBuffer);
	CLAIR_ASSERT(!FAILED(result), "Viewport resize error");

	releaseComObject(depthStencilView);
	result = d3dDevice->CreateDepthStencilView(depthStencilBuffer, nullptr,
											   &depthStencilView);
	CLAIR_ASSERT(!FAILED(result), "Viewport resize error");

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

void Renderer::render(Scene* const scene) {
	CLAIR_ASSERT(scene, "Scene should not be null");
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
	ID3D11ShaderResourceView* bla[] {nullptr, nullptr, nullptr, nullptr};
	d3dDeviceContext->PSSetShaderResources(0, 4, bla);
}

void Renderer::setViewMatrix(const Float4x4& view) {
	cameraView = view;
}

void Renderer::setProjectionMatrix(const Float4x4& projection) {
	cameraProjection = projection;
}

void Renderer::setRenderPass(const RenderPass pass) {
	gRenderPass = pass;
}

void Renderer::setCameraPosition(const Float3& position) {
	cameraPosition = position;
}

void Renderer::renderScreenQuad(
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
	d3dDeviceContext->PSSetSamplers(0, 1, &samplerState);

	const auto matCbData = materialInstance->getConstBufferData()->getDataPs();
	const auto matCb = mat->getConstantBufferPs();
	if (matCb && matCb->isValid()) {
		const auto matD3d = matCb->getD3dBuffer();
		d3dDeviceContext->UpdateSubresource(matD3d, 0, nullptr, matCbData,
											0, 0);
		d3dDeviceContext->PSSetConstantBuffers(1, 1, &matD3d);
	}
	const auto texMap = materialInstance->getTextureMap();
	for (const auto& itTex : texMap) {
		auto const resView = itTex.second->getD3DShaderResourceView();
		d3dDeviceContext->PSSetShaderResources(itTex.first, 1, &resView);
	}
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
	ID3D11ShaderResourceView* bla[] {nullptr, nullptr, nullptr, nullptr};
	d3dDeviceContext->PSSetShaderResources(0, 4, bla);
}