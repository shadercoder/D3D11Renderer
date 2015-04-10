#include "SampleFramework/SampleFramework.h"
#include <Windows.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "SampleFramework/Input.h"
#include "SampleFramework/Timer.h"
#include "SampleFramework/SampleBase.h"

using namespace SampleFramework;

static SampleBase* gSample {nullptr};
static SDL_Window* SDL_window {nullptr};
static bool isRunning {true};

static void handleEvents() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				gSample->onResize(event.window.data1, event.window.data2);
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

bool Framework::run(SampleBase* const sample, const std::string& caption) {
	gSample = sample;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		MessageBox(nullptr, "Couldn't initialize SDL2", "Fatal error",
				   MB_OK | MB_ICONERROR);
		return false;
	}
	
	SDL_window = SDL_CreateWindow(caption.c_str(), SDL_WINDOWPOS_CENTERED,
								  SDL_WINDOWPOS_CENTERED, 640, 480,
								  SDL_WINDOW_RESIZABLE);
	if (!SDL_window) {
		MessageBox(nullptr, (std::string("Couldn't initialize SDL2:\n") +
				   SDL_GetError()).c_str(), "Fatal error",
				   MB_OK | MB_ICONERROR);
		SDL_Quit();
		return false;
	}
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(SDL_window, &info);

	if (!gSample->initialize(info.info.win.window)) {
		MessageBox(nullptr, "Couldn't initialize Clair.",
				   "Fatal error", MB_OK | MB_ICONERROR);
		gSample->terminate();
		SDL_Quit();
		return false;
	}

	Timer timer;
	timer.start();
	double deltaTime = 0.0f;
	double runningTime = 0.0f;
	while (isRunning) {
		Input::update();
		handleEvents();
		if (Input::getKey(SDL_SCANCODE_ESCAPE)) isRunning = false;

		gSample->update(static_cast<float>(deltaTime),
						static_cast<float>(runningTime));
		gSample->render();

		const double elapsedTime = timer.elapsed();
		deltaTime = elapsedTime;
		runningTime += elapsedTime;
		timer.start();
	}

	gSample->terminate();

	SDL_Quit();
	return true;
}
