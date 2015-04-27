#pragma once
#include "SDL2/SDL.h"

namespace SampleFramework {
	class Gui {
	public:
		static bool initialize();
		static void terminate();

		static void resize(int width, int height);
		static void newFrame();
		static void render();
		static void handleEvent(const SDL_Event& event);
	};
}