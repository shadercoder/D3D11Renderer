#include "Clair/Clair.h"

using namespace Clair;

bool Clair::initialize(const HWND hwnd, const LogCallback logCallback) {
	if (!Renderer::initialize(hwnd)) {
		return false;
	}
	if (!ResourceManager::initialize()) {
		return false;
	}
	Log::setCallback(logCallback);
	CLAIR_DEBUG_LOG("Clair initialized");
	return true;
}

void Clair::terminate() {
	ResourceManager::terminate();
	Renderer::terminate();
	CLAIR_DEBUG_LOG("Clair terminated");
}