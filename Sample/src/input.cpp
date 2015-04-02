#include "input.h"
#include <memory>

bool Input::msKeys[] = { false };
bool Input::msPrevKeys[] = { false };

void Input::initialize() {
	std::fill(msKeys, msKeys + SDL_NUM_SCANCODES, false);
	std::fill(msPrevKeys, msPrevKeys + SDL_NUM_SCANCODES, false);
}

void Input::update() {
	std::copy(msKeys, msKeys + SDL_NUM_SCANCODES, msPrevKeys);
}

void Input::setKeyDown(const SDL_Scancode scanCode) {
	msKeys[scanCode] = true;
}

void Input::setKeyUp(SDL_Scancode scanCode) {
	msKeys[scanCode] = false;
}

bool Input::getKey(const SDL_Scancode scancode) {
	return msKeys[scancode];
}

bool Input::getKeyDown(const SDL_Scancode scancode) {
	return msKeys[scancode] && !msPrevKeys[scancode];
}

bool Input::getKeyUp(SDL_Scancode scancode) {
	return !msKeys[scancode] && msPrevKeys[scancode];
}
