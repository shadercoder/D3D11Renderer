#pragma once
#include "clair/matrix.h"

namespace Clair {
	class Node {
	public:
		Node() : mMatrix(Matrix()) {}

		Matrix getMatrix() const;
		void setMatrix(const Matrix& m);

	private:
		Matrix mMatrix;
	};

	inline Matrix Node::getMatrix() const {
		return mMatrix;
	}
	
	inline void Node::setMatrix(const Matrix& m) {
		mMatrix = m;
	}

	class Object : public Node {
	public:	

	};
}

