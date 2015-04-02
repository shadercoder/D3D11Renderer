#pragma once
#include "SDL2/SDL.h"

class Input {
public:
	static void initialize();

	static void update();

	static void setKeyDown(SDL_Scancode scanCode);
	static void setKeyUp(SDL_Scancode scanCode);

	static bool getKey(SDL_Scancode scancode);
	static bool getKeyDown(SDL_Scancode scancode);
	static bool getKeyUp(SDL_Scancode scancode);

private:
	static bool msKeys[SDL_NUM_SCANCODES];
	static bool msPrevKeys[SDL_NUM_SCANCODES];
};