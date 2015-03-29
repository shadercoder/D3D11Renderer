#include <Windows.h>
#include "sample.h"
#include "clair/renderer.h"
//#include "vld.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:wWinMainCRTStartup")
#endif

HWND hwnd;
float windowWidth = 0.0f;
float windowHeight = 0.0f;
bool resizeWin = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_CHAR:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE: {
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		windowWidth = static_cast<float>(clientRect.right - clientRect.left);
		windowHeight = static_cast<float>(clientRect.bottom - clientRect.top);
		resizeWin = true;
		break;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	return 0;
}

bool initializeWindow(HINSTANCE hInstance, const int cmdShow) {
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = "TheWindowClass";
	if (!RegisterClassEx(&wndClass)) return false; 
	RECT rc = { 0, 0, 640, 480 };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hwnd = CreateWindowA(	"TheWindowClass", "Clair renderer sample",
							WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							rc.right - rc.left, rc.bottom - rc.top,
							NULL, NULL, hInstance, NULL);

	if(!hwnd) {
		return false;
	}

	ShowWindow(hwnd, cmdShow);
	return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, const int cmdShow) { // UNREFERENCED PARAMETER
	if (!initializeWindow(hInstance, cmdShow)) {
		MessageBox(nullptr, "Couldn't open a window.", "Fatal error",
				   MB_OK | MB_ICONERROR);
		return -1;
	}

	;
	if (!Clair::Renderer::initialize(hwnd, "../../data/ClairData")) {
		MessageBox(nullptr, "Couldn't initialize the rendering engine.",
				   "Fatal error", MB_OK | MB_ICONERROR);
		Clair::Renderer::terminate();
		return -1;
	}

	Sample sample;	
	sample.initialize();

	MSG msg = { 0 };
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (resizeWin) {
				sample.onResize(windowWidth, windowHeight);
				resizeWin = false;
			}
			sample.update();
			sample.render();
			Clair::Renderer::finalizeFrame();
		}
	}

	sample.terminate();
	Clair::Renderer::terminate();

	return static_cast<int>(msg.wParam);
}