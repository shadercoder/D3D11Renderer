#include "SampleFramework/Camera.h"
#include "SampleFramework/Input.h"

using namespace glm;
using namespace SampleFramework;

mat4 Camera::msViewMatrix {};
vec3 Camera::msPosition {0.0f};
vec3 Camera::msForward {0.0f};
vec3 Camera::msRight {0.0f};
vec3 Camera::msUp {0.0f};
vec2 Camera::msSnapPos {0.0f};
bool Camera::msIsSnapping {false};
float Camera::msPitch {0.0f};
float Camera::msYaw {0.0f};

void Camera::initialize(const glm::vec3& position,
						const float pitch, const float yaw) {
	msPosition = position;
	msPitch = pitch;
	msYaw = yaw;
}

void Camera::update(const float deltaTime) {
	const float speed {2.0f *
					   (Input::getKey(SDL_SCANCODE_LSHIFT) ? 4.0f : 1.0f) *
					   deltaTime};
	if (Input::getKey(SDL_SCANCODE_W)) msPosition += msForward * speed;
	if (Input::getKey(SDL_SCANCODE_S)) msPosition -= msForward * speed;
	if (Input::getKey(SDL_SCANCODE_D)) msPosition += msRight * speed;
	if (Input::getKey(SDL_SCANCODE_A)) msPosition -= msRight * speed;
	if (Input::getKey(SDL_SCANCODE_SPACE)) msPosition += msUp * speed;
	if (Input::getKey(SDL_SCANCODE_LCTRL)
		|| Input::getKey(SDL_SCANCODE_C)) msPosition -= msUp * speed;

	if (Input::getMouseButton(SDL_BUTTON_LEFT)) {
		SDL_ShowCursor(0);
		ivec2 icursor;
		SDL_GetMouseState(&icursor.x, &icursor.y);
		vec2 cursor (static_cast<int>(icursor.x), static_cast<int>(icursor.y));
		if (!msIsSnapping) {
			msSnapPos = cursor;
			msIsSnapping = true;
		} else {
			if (cursor != msSnapPos) {
				const float rotSpeed {0.005f};
				msPitch += (cursor.y - msSnapPos.y) * rotSpeed;
				msYaw += (cursor.x - msSnapPos.x) * rotSpeed;
				const float maxPitch{90.0f / 180.0f * static_cast<float>(M_PI)};
				msPitch = max(min(msPitch, maxPitch), -maxPitch);
				SDL_WarpMouseInWindow(nullptr, static_cast<int>(msSnapPos.x),
									  static_cast<int>(msSnapPos.y));
			}
		}
	} else {
		msIsSnapping = false;
		SDL_ShowCursor(1);
	}
	updateMatrix();
}

void Camera::updateMatrix() {
	const float cosPitch {cos(msPitch)};
	const float sinPitch {sin(msPitch)};
	const float cosYaw   {cos(msYaw)};
	const float sinYaw   {sin(msYaw)};
 
	msRight = vec3(cosYaw, 0, -sinYaw);
	msUp = vec3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
	msForward = vec3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

	msViewMatrix = mat4(vec4(msRight.x, msUp.x, msForward.x, 0),
						vec4(msRight.y, msUp.y, msForward.y, 0),
						vec4(msRight.z, msUp.z, msForward.z, 0),
						vec4(-dot(msRight, msPosition), -dot(msUp, msPosition),
							 -dot(msForward, msPosition), 1.0f));
}

mat4 Camera::getViewMatrix() {
	return msViewMatrix;
}
