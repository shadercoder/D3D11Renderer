#pragma once
#include "GlmMath.h"

class Camera {
public:
	static void update(float deltaTime);
	static glm::mat4 getViewMatrix();

private:
	static void updateMatrix();
	static glm::mat4 msViewMatrix;
	static glm::vec3 msPosition;
	static glm::vec3 msForward;
	static glm::vec3 msRight;
	static glm::vec2 msSnapPos;
	static bool msIsSnapping;
	static float msPitch;
	static float msYaw;
};