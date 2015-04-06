#pragma once
#include <memory>

namespace Clair {
	class Matrix {
	public:
		inline Matrix() {
			memset(&m[0], 0, 4 * 4 * sizeof(float));
		}

		inline Matrix(const Matrix& other) {
			memcpy(&(m[0]), &other.m[0], 4 * 4 * sizeof(float));
		}

		inline Matrix& operator=(const Matrix& other) {
			memcpy(&(m[0]), &other.m[0], 4 * 4 * sizeof(float));
			return *this;
		}

		inline Matrix(const float* data) {
			memcpy(&(m[0]), data, 4 * 4 * sizeof(float));
		}

	private:
		float m[16];
	};
}