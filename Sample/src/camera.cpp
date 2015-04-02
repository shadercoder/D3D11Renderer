#include "camera.h"
#include "input.h"

using namespace glm;

vec3 Camera::msPosition = vec3(0.0f, 0.0f, 0.0f);

void Camera::update(float deltaTime) {
	const float speed = Input::getKey(SDL_SCANCODE_LSHIFT) ?
						3.0f * deltaTime :
						1.0f * deltaTime;
	if (Input::getKey(SDL_SCANCODE_W))  msPosition += vec3(0.0f, 0.0f,  speed);
	if (Input::getKey(SDL_SCANCODE_S))  msPosition += vec3(0.0f, 0.0f, -speed);
	if (Input::getKey(SDL_SCANCODE_A))  msPosition += vec3(-speed, 0.0f, 0.0f);
	if (Input::getKey(SDL_SCANCODE_D))  msPosition += vec3( speed, 0.0f,  0.0f);
	if (Input::getKey(SDL_SCANCODE_LCTRL))
		msPosition += vec3(0.0f, -speed, 0.0f);
	if (Input::getKey(SDL_SCANCODE_SPACE))
		msPosition += vec3(0.0f, speed,  0.0f);
}

glm::mat4 Camera::getViewMatrix() {
	return inverse(translate(msPosition));
}

