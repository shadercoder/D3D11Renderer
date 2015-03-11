#pragma once
#include "clair/matrix.h"
#include "clair/vertexBuffer.h"

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

	class Mesh;
	class Object : public Node {
	public:	
		void setMesh(Mesh* mesh) { mMesh = mesh; }
		const Mesh* getMesh() const { return mMesh; }
		
	private:
		Mesh* mMesh = nullptr;
	};
}

