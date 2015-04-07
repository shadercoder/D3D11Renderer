#include "SampleCommon/Input.h"

using namespace SampleCommon;

std::array<bool, SDL_NUM_SCANCODES> Input::msKeys;
std::array<bool, SDL_NUM_SCANCODES> Input::msPrevKeys;
std::array<bool, 8> Input::msButtons;
std::array<bool, 8> Input::msPrevButtons;

void Input::initialize() {
	std::fill(msKeys.begin(), msKeys.end(), false);
	std::fill(msPrevKeys.begin(), msPrevKeys.end(), false);
	std::fill(msButtons.begin(), msButtons.end(), false);
	std::fill(msPrevButtons.begin(), msPrevButtons.end(), false);
}

void Input::update() {
	std::copy(msKeys.begin(), msKeys.end(), msPrevKeys.begin());
	std::copy(msButtons.begin(), msButtons.end(), msPrevKeys.begin());
}

void Input::setKeyDown(const SDL_Scancode scanCode) {
	msKeys[scanCode] = true;
}

void Input::setKeyUp(const SDL_Scancode scanCode) {
	msKeys[scanCode] = false;
}

void Input::setMouseButtonDown(const Uint8 button) {
	msButtons[button] = true;
}

void Input::setMouseButtonUp(const Uint8 button) {
	msButtons[button] = false;
}

bool Input::getKey(const SDL_Scancode scancode) {
	return msKeys[scancode];
}

bool Input::getKeyDown(const SDL_Scancode scancode) {
	return msKeys[scancode] && !msPrevKeys[scancode];
}

bool Input::getKeyUp(const SDL_Scancode scancode) {
	return !msKeys[scancode] && msPrevKeys[scancode];
}

bool Input::getMouseButton(const Uint8 button) {
	return msButtons[button];
}

bool Input::getMouseButtonDown(const Uint8 button) {
	return msButtons[button] && !msPrevButtons[button];
}

bool Input::getMouseButtonUp(const Uint8 button) {
	return !msButtons[button] && msPrevButtons[button];
}