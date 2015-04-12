#pragma once
#pragma warning(push, 3)
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

// I like LH perspective matrices and this version of GLM didn't have a function
// for that yet...
namespace glm {
	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> perspectiveLH
	(
		T fovy,
		T aspect,
		T zNear,
		T zFar
	)
	{
		assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

		T const tanHalfFovy = tan(fovy / static_cast<T>(2));

		tmat4x4<T, defaultp> Result(static_cast<T>(0));
		Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
		Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
		Result[2][2] = - (zFar + zNear) / (zNear - zFar);
		Result[2][3] = static_cast<T>(1);
		Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
		return Result;
	}
}
#pragma warning(pop)