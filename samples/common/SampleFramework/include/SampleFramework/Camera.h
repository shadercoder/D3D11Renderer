#pragma once
#include "GlmMath.h"

namespace SampleFramework {
	class Camera {
	public:
		static void initialize(const glm::vec3& position,
							   float pitch, float yaw);
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
}