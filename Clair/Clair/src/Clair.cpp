#include "Clair/Clair.h"
#include "D3dDevice.h"

using namespace Clair;

bool Clair::initialize(const HWND hwnd, const LogCallback logCallback) {
	Log::setCallback(logCallback);
	if (!D3dDevice::initialize(hwnd)) {
		return false;
	}
	if (!ResourceManager::initialize()) {
		return false;
	}
	if (!Renderer::initialize(hwnd)) {
		return false;
	}
	CLAIR_DEBUG_LOG("Clair initialized");
	return true;
}

void Clair::terminate() {
	Renderer::terminate();
	ResourceManager::terminate();
	D3dDevice::terminate();
	CLAIR_DEBUG_LOG("Clair terminated");
}