#pragma once
#include "GlmMath.h"

namespace SampleFramework {
	class Camera {
	public:
		static void initialize(const glm::vec3& position,
							   float pitch, float yaw);
		static void update(float deltaTime);
		static glm::mat4 getViewMatrix();
		static glm::vec3 getPosition();
		static glm::vec3 getRight();
		static glm::vec3 getUp();
		static glm::vec3 getForward();

	private:
		static void updateMatrix();
		static glm::mat4 msViewMatrix;
		static glm::vec3 msPosition;
		static glm::vec3 msForward;
		static glm::vec3 msRight;
		static glm::vec3 msUp;
		static glm::vec2 msSnapPos;
		static bool msIsSnapping;
		static float msPitch;
		static float msYaw;
	};

	inline glm::vec3 Camera::getPosition() {
		return msPosition;
	}

	inline glm::vec3 Camera::getRight() {
		return msRight;
	}

	inline glm::vec3 Camera::getUp() {
		return msUp;
	}

	inline glm::vec3 Camera::getForward() {
		return msForward;
	}
}