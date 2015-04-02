#pragma once
#include "glmMath.h"

class Camera {
public:
	static void update(float deltaTime);
	static glm::mat4 getViewMatrix();

private:
	static glm::vec3 msPosition;
};