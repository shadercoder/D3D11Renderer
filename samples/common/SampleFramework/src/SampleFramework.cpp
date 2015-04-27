#include "SampleFramework/SampleFramework.h"
#include <Windows.h>
#include <GL/GL.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "SampleFramework/Input.h"
#include "SampleFramework/Timer.h"
#include "SampleFramework/SampleBase.h"
#include "SampleFramework/Loader.h"
#include "SampleFramework/Random.h"
#include "Gui.h"

using namespace SampleFramework;

static SampleBase* gSample {nullptr};
static SDL_Window* gMainWindow {nullptr};
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
		//if (event.window.windowID == 2) {
			Gui::handleEvent(event);
		//	continue;
		//}
		switch(event.type) {
		case SDL_QUIT:
			gIsRunning = false;
			break;
		case SDL_WINDOWEVENT_CLOSE:
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

int Framework::run(SampleBase* const sample, const std::string& caption,
					const int width, const int height,
					const std::string& dataPath) {
	Logger::reset(0.0f);
	setWindowSize(width, height);
	gSample = sample;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		MessageBox(nullptr, "Couldn't initialize SDL2", "Fatal error",
				   MB_OK | MB_ICONERROR);
		delete sample;
		return -1;
	}
	
	gMainWindow = SDL_CreateWindow(caption.c_str(), SDL_WINDOWPOS_CENTERED,
								  SDL_WINDOWPOS_CENTERED, gWidth, gHeight,
								  SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
								  | SDL_WINDOW_OPENGL);
	if (!gMainWindow) {
		MessageBox(nullptr, (std::string("Couldn't initialize SDL2:\n") +
				   SDL_GetError()).c_str(), "Fatal error",
				   MB_OK | MB_ICONERROR);
		SDL_Quit();
		delete sample;
		return -1;
	}

	// Initialize all systems
	Logger::log("SampleFramework: Main window created");
	Loader::setSearchPath(dataPath);
	Random::initialize();
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(gMainWindow, &info);
	int winX, winY;
	SDL_GetWindowPosition(gMainWindow, &winX, &winY);
	// OpenGL GUI window
	RECT rect;
	GetWindowRect(info.info.win.window, &rect);
	const int borderWidth = rect.right - rect.left - gWidth;
	const int guiWidth = 300;
	SDL_Window* guiWindow =
		SDL_CreateWindow("Clair sample GUI", winX - (guiWidth + borderWidth),
						 winY, guiWidth, gHeight, SDL_WINDOW_OPENGL);
	SDL_GLContext glcontext = SDL_GL_CreateContext(guiWindow);
	SDL_GL_SetSwapInterval(0);
	SDL_ShowWindow(gMainWindow); // show main window later to give it focus
	Logger::log("SampleFramework: GUI window created");
	Gui::initialize();
	Gui::resize(guiWidth, gHeight);
	Logger::log("SampleFramework: GUI initialized");
	if (!gSample->initialize(info.info.win.window)) {
		MessageBox(nullptr, "Couldn't initialize sample.",
				   "Fatal error", MB_OK | MB_ICONERROR);
		gSample->terminate();
		SDL_Quit();
		delete sample;
		return -1;
	}
	Logger::log("SampleFramework: Sample initialized");

	gSample->onResize(gWidth, gHeight, gAspect);

	Timer timer;
	timer.start();
	double deltaTime = 0.0f;
	double runningTime = 0.0f;
	while (gIsRunning) {
		// Events and input
		Gui::newFrame();
		Input::update();
		handleEvents();
		if (Input::getKey(SDL_SCANCODE_ESCAPE)) gIsRunning = false;

		// Sample
		gSample->update();
		gSample->render();

		// GUI
		Gui::render();
		//SwapBuffers(GetDC(info.info.win.window));
		SDL_GL_SwapWindow(guiWindow);

		// Timing
		const double elapsedTime = timer.elapsed();
		deltaTime = elapsedTime;
		runningTime += elapsedTime;
		gSample->mDeltaTime = static_cast<float>(deltaTime);
		gSample->mRunningTime = static_cast<float>(runningTime);
		Logger::reset(runningTime);
		timer.start();
	}

	gSample->terminate();
	Logger::log("SampleFramework: Sample terminated");
	Gui::terminate();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(guiWindow);
	SDL_DestroyWindow(gMainWindow);
	SDL_Quit();
	delete sample;
	Logger::log("SampleFramework: Terminated");
	return 0;
}
