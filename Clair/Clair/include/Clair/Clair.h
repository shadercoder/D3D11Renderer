#pragma once
#include "Clair/Renderer.h"
#include "Clair/ResourceManager.h"
#include "Clair/Debug.h"

struct HWND__;
typedef HWND__* HWND;

namespace Clair {
	bool initialize(HWND hwnd, LogCallback logCallback);
	void terminate();
}