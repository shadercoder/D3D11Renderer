#pragma once
#include <d3d11.h>

struct ID3D11Device;

namespace Clair {
	class D3dDevice {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static ID3D11Device* getD3dDevice();
		static ID3D11DeviceContext* getD3dDeviceContext();
		static IDXGISwapChain* getSwapChain();

	private:
		static ID3D11Device* msD3dDevice;
		static ID3D11DeviceContext* msD3dDeviceContext;
		static IDXGISwapChain* msSwapChain;
	};
	
	inline ID3D11Device* D3dDevice::getD3dDevice() {
		return msD3dDevice;
	}

	inline ID3D11DeviceContext* D3dDevice::getD3dDeviceContext() {
		return msD3dDeviceContext;
	}

	inline IDXGISwapChain* D3dDevice::getSwapChain() {
		return msSwapChain;
	}
}