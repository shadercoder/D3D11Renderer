#include "SampleFramework/SampleFramework.h"
#include <Windows.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "SampleFramework/Input.h"
#include "SampleFramework/Timer.h"
#include "SampleFramework/SampleBase.h"

using namespace SampleFramework;

static SampleBase* gSample {nullptr};
static SDL_Window* gSDL_window {nullptr};
static bool gIsRunning {true};
static int gWidth {0};
static int gHeight {0};
static float gAspect {0.0f};

static void setWindowSize(const int width, const int height) {
	gWidth = width;
	gHeight = height;
	gAspect = static_cast<float>(gWidth) / static_cast<float>(gHeight);
}

static void handleEvents() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_QUIT:
			gIsRunning = false;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				setWindowSize(event.window.data1, event.window.data2);
				gSample->onResize(gWidth, gHeight, gAspect);
			}
			break;
		case SDL_KEYDOWN:
			Input::setKeyDown(event.key.keysym.scancode);
			break;
		case SDL_KEYUP:
			Input::setKeyUp(event.key.keysym.scancode);
			break;
		case SDL_MOUSEBUTTONDOWN:
			Input::setMouseButtonDown(event.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			Input::setMouseButtonUp(event.button.button);
			break;
		default:
			break;
		}
	}
}

bool Framework::run(SampleBase* const sample, const std::string& caption,
					const int width, const int height) {
	setWindowSize(width, height);
	gSample = sample;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		MessageBox(nullptr, "Couldn't initialize SDL2", "Fatal error",
				   MB_OK | MB_ICONERROR);
		return false;
	}
	
	gSDL_window = SDL_CreateWindow(caption.c_str(), SDL_WINDOWPOS_CENTERED,
								  SDL_WINDOWPOS_CENTERED, gWidth, gHeight,
								  SDL_WINDOW_RESIZABLE);
	if (!gSDL_window) {
		MessageBox(nullptr, (std::string("Couldn't initialize SDL2:\n") +
				   SDL_GetError()).c_str(), "Fatal error",
				   MB_OK | MB_ICONERROR);
		SDL_Quit();
		return false;
	}
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(gSDL_window, &info);

	if (!gSample->initialize(info.info.win.window)) {
		MessageBox(nullptr, "Couldn't initialize Clair.",
				   "Fatal error", MB_OK | MB_ICONERROR);
		gSample->terminate();
		SDL_Quit();
		return false;
	}

	gSample->onResize(gWidth, gHeight, gAspect);

	Timer timer;
	timer.start();
	double deltaTime = 0.0f;
	double runningTime = 0.0f;
	while (gIsRunning) {
		Input::update();
		handleEvents();
		if (Input::getKey(SDL_SCANCODE_ESCAPE)) gIsRunning = false;

		gSample->update();
		gSample->render();

		const double elapsedTime = timer.elapsed();
		deltaTime = elapsedTime;
		runningTime += elapsedTime;
		gSample->mDeltaTime = static_cast<float>(deltaTime);
		gSample->mRunningTime = static_cast<float>(runningTime);
		timer.start();
	}

	gSample->terminate();

	SDL_Quit();
	return true;
}
