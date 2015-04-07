#pragma once
#include "SDL2/SDL.h"
#include <array>

namespace SampleCommon {
	class Input {
	public:
		static void initialize();

		static void update();

		static void setKeyDown(SDL_Scancode scanCode);
		static void setKeyUp(SDL_Scancode scanCode);
		static void setMouseButtonDown(Uint8 button);
		static void setMouseButtonUp(Uint8 button);

		static bool getKey(SDL_Scancode scancode);
		static bool getKeyDown(SDL_Scancode scancode);
		static bool getKeyUp(SDL_Scancode scancode);
		static bool getMouseButton(Uint8 button);
		static bool getMouseButtonDown(Uint8 button);
		static bool getMouseButtonUp(Uint8 button);

	private:
		static std::array<bool, SDL_NUM_SCANCODES> msKeys;
		static std::array<bool, SDL_NUM_SCANCODES> msPrevKeys;
		static std::array<bool, 8> msButtons;
		static std::array<bool, 8> msPrevButtons;
	};
}