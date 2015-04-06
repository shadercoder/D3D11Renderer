#include <Windows.h>
#include "Sample.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "Input.h"
#include "Timer.h"
//#include "vld.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

static Sample sample;	
static SDL_Window*	SDL_window = nullptr;
static bool isRunning = true;

static void handleEvents();

int main(int, char*[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		MessageBox(nullptr, "Couldn't initialize SDL2", "Fatal error",
				   MB_OK | MB_ICONERROR);
		return -1;
	}
	
	SDL_window = SDL_CreateWindow("Clair sample", SDL_WINDOWPOS_CENTERED,
								  SDL_WINDOWPOS_CENTERED, 640, 480,
								  SDL_WINDOW_RESIZABLE);
	if (!SDL_window) {
		MessageBox(nullptr, (std::string("Couldn't initialize SDL2:\n") +
				   SDL_GetError()).c_str(), "Fatal error",
				   MB_OK | MB_ICONERROR);
		SDL_Quit();
		return 1;
	}
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(SDL_window, &info);

	if (!sample.initialize(info.info.win.window)) {
		MessageBox(nullptr, "Couldn't initialize Clair.",
				   "Fatal error", MB_OK | MB_ICONERROR);
		sample.terminate();
		SDL_Quit();
		return -1;
	}

	Timer timer;
	timer.start();
	double deltaTime = 0.0f;
	double runningTime = 0.0f;
	while (isRunning) {
		Input::update();
		handleEvents();
		if (Input::getKey(SDL_SCANCODE_ESCAPE)) isRunning = false;

		sample.update(static_cast<float>(deltaTime),
					  static_cast<float>(runningTime));
		sample.render();

		const double elapsedTime = timer.elapsed();
		deltaTime = elapsedTime;
		runningTime += elapsedTime;
		timer.start();
	}

	sample.terminate();

	SDL_Quit();
	return 0;
}

 void handleEvents() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				sample.onResize(event.window.data1, event.window.data2);
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