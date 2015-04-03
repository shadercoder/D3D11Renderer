#include "camera.h"
#include "input.h"

using namespace glm;

mat4 Camera::msViewMatrix = mat4();
vec3 Camera::msPosition = vec3(0.0f);
vec3 Camera::msForward = vec3(0.0f);
vec3 Camera::msRight = vec3(0.0f);
vec2 Camera::msSnapPos = vec2(0.0f);
bool Camera::msIsSnapping = false;
float Camera::msPitch = 0.0f;
float Camera::msYaw = 0.0f;

void Camera::update(float deltaTime) {
	const float speed = Input::getKey(SDL_SCANCODE_LSHIFT) ?
						20.0f * deltaTime :
						5.0f * deltaTime;
	if (Input::getKey(SDL_SCANCODE_W)) msPosition += msForward * speed;
	if (Input::getKey(SDL_SCANCODE_S)) msPosition -= msForward * speed;
	if (Input::getKey(SDL_SCANCODE_D)) msPosition += msRight * speed;
	if (Input::getKey(SDL_SCANCODE_A)) msPosition -= msRight * speed;
	if (Input::getKey(SDL_SCANCODE_LCTRL))
		msPosition += vec3(0.0f, -speed, 0.0f);
	if (Input::getKey(SDL_SCANCODE_SPACE))
		msPosition += vec3(0.0f, speed,  0.0f);

	if (Input::getMouseButton(SDL_BUTTON_LEFT)) {
		SDL_ShowCursor(0);
		ivec2 icursor;
		SDL_GetMouseState(&icursor.x, &icursor.y);
		vec2 cursor (static_cast<int>(icursor.x), static_cast<int>(icursor.y));
		if (!msIsSnapping) msSnapPos = cursor;
		msIsSnapping = true;

		if (cursor != msSnapPos) {
			const float rotSpeed = 0.005f;
			msPitch += (cursor.y - msSnapPos.y) * rotSpeed;
			msYaw += (cursor.x - msSnapPos.x) * rotSpeed;
			const float maxPitch = 90.0f / 180.0f * static_cast<float>(M_PI);
			msPitch = max(min(msPitch, maxPitch), -maxPitch);
		}
		SDL_WarpMouseInWindow(nullptr, static_cast<int>(msSnapPos.x),
							  static_cast<int>(msSnapPos.y));
	} else {
		msIsSnapping = false;
		SDL_ShowCursor(1);
	}
	updateMatrix();
}

void Camera::updateMatrix() {
	const float cosPitch = cos(msPitch);
	const float sinPitch = sin(msPitch);
	const float cosYaw   = cos(msYaw);
	const float sinYaw   = sin(msYaw);
 
	msRight = vec3(cosYaw, 0, -sinYaw);
	const vec3 yaxis = vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
	msForward = vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

	msViewMatrix  = mat4(vec4(msRight.x, yaxis.x, msForward.x, 0),
						 vec4(msRight.y, yaxis.y, msForward.y, 0),
						 vec4(msRight.z, yaxis.z, msForward.z, 0),
						 vec4(-dot(msRight, msPosition), -dot(yaxis, msPosition),
							  -dot(msForward, msPosition), 1.0f));
}

mat4 Camera::getViewMatrix() {
	return msViewMatrix;
}

