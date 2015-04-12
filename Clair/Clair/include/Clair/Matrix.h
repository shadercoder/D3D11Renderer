#pragma once
#include <memory>

namespace Clair {
	template<typename T, size_t x, size_t y>
	class Matrix {
	public:
		static_assert(x >= 1 && x <= 4 && y >=1 && y <= 4,
					  "Clair::Matrix dimensions must be between 1x1 and 4x4");

		Matrix();
		Matrix(const T* data);
		Matrix(const Matrix& other);
		Matrix<T, x, y>& operator=(const Matrix<T, x, y>& other);
		T* operator[](int index);

	private:
		T m[x][y];
	};

	typedef Matrix<float, 1, 2> Float2;
	typedef Matrix<float, 1, 3> Float3;
	typedef Matrix<float, 1, 4> Float4;
	typedef Matrix<float, 2, 4> Float2x4;
	typedef Matrix<float, 3, 4> Float3x4;
	typedef Matrix<float, 4, 4> Float4x4;

	template<typename T, size_t x, size_t y>
	inline Matrix<T, x, y>::Matrix() {
		std::fill(&m[0][0], &m[0][0] + x * y, T());
	}

	template<typename T, size_t x, size_t y>
	inline Matrix<T, x, y>::Matrix(const T* const data) {
		memcpy(&m, data, x * y * sizeof(T));
	}

	template<typename T, size_t x, size_t y>
	inline Matrix<T, x, y>::Matrix(const Matrix& other) {
		memcpy(&m, &other.m, x * y * sizeof(T));
	}

	template<typename T, size_t x, size_t y>
	inline Matrix<T, x, y>&
	Matrix<T, x, y>::operator=(const Matrix<T, x, y>& other) {
		memcpy(&m, &other.m, x * y * sizeof(T));
		return *this;
	}

	template<typename T, size_t x, size_t y>
	inline T*
	Matrix<T, x, y>::operator[](const int index) {
		return m[index];
	}
}