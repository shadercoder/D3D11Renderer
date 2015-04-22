#include "D3dDevice.h"

using namespace Clair;

ID3D11Device* D3dDevice::msD3dDevice {nullptr};
ID3D11DeviceContext* D3dDevice::msD3dDeviceContext {nullptr};
IDXGISwapChain* D3dDevice::msSwapChain {nullptr};

bool D3dDevice::initialize(const HWND hwnd) {
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
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
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
										   &msSwapChain, &msD3dDevice,
										   &featureLevel, &msD3dDeviceContext);

	return !FAILED(result);
}

void D3dDevice::terminate() {
	msSwapChain->Release();
	msD3dDeviceContext->Release();
	msD3dDevice->Release();
}