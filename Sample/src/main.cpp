#include <Windows.h>
#include "clair/renderer.h"
#include "clair/scene.h"
#include "clair/object.h"
#include "clair/camera.h"
//#include "vld.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:wWinMainCRTStartup")
#endif

HWND hwnd;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT paintStruct;
	HDC hDC;
	switch(message) {
	case WM_CHAR:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
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
							WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
							rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if(!hwnd) {
		return false;
	}

	ShowWindow(hwnd, cmdShow);
	return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int cmdShow) {
	if (!initializeWindow(hInstance, cmdShow)) {
		MessageBox(nullptr, "Couldn't open a window.", "Fatal error", MB_OK|MB_ICONERROR);
		return -1;
	}

	;
	if (!Clair::Renderer::initialize(hwnd, "../data/ClairData")) {
		MessageBox(nullptr, "Couldn't initialize the rendering engine.", "Fatal error", MB_OK|MB_ICONERROR);
		Clair::Renderer::terminate();
		return -1;
	}

	Clair::Renderer::setViewport(0, 0, 640, 480);

	float m[16] = {	1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 2.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, -1.0f, 0.0f, 1.0f };
	Clair::Scene* scene = Clair::Renderer::createScene();
	Clair::Object* cube0 = scene->createObject();
	cube0->setMatrix(Clair::Matrix(m));
	Clair::Object* cube1 = scene->createObject();
	m[12] += 4.0f;
	cube1->setMatrix(Clair::Matrix(m));
	Clair::Object* cube2 = scene->createObject();
	m[14] += 4.0f;
	cube2->setMatrix(Clair::Matrix(m));
	Clair::Object* cube3 = scene->createObject();
	m[12] -= 4.0f;
	cube3->setMatrix(Clair::Matrix(m));
	Clair::Camera* camera = scene->createCamera();
	camera->setMatrix(Clair::Matrix());

	MSG msg = { 0 };
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Clair::Renderer::render();
		}
	}

	Clair::Renderer::terminate();

	return static_cast<int>(msg.wParam);
}