#include <Windows.h>
#include "sample.h"
#include "clair/renderer.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "vld.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

Sample sample;	
SDL_Window*	SDL_window = nullptr;
bool isRunning = true;

void handleEvents();

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

	if (!Clair::Renderer::initialize(info.info.win.window)) {
		MessageBox(nullptr, "Couldn't initialize Clair.",
				   "Fatal error", MB_OK | MB_ICONERROR);
		Clair::Renderer::terminate();
		SDL_Quit();
		return -1;
	}

	sample.initialize();

	while (isRunning) {
		handleEvents();
		sample.update();
		sample.render();
		Clair::Renderer::finalizeFrame();
	}

	sample.terminate();
	Clair::Renderer::terminate();

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
				sample.onResize(static_cast<float>(event.window.data1),
								static_cast<float>(event.window.data2));
			}
			break;
		default:
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				isRunning = false;
			}
			break;
		}
	}
}