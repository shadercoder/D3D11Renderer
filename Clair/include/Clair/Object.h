#pragma once
#include "Clair/Matrix.h"
#include "Clair/RenderPass.h"
#include <map>

namespace Clair {
	class Mesh;
	class Material;

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
		void setMesh(Mesh* mesh) { mMesh = mesh; }
		const Mesh* getMesh() const { return mMesh; }

		void setMaterial(RenderPass pass, Clair::Material* subMaterial);
		Clair::Material* getMaterial(RenderPass pass);

	private:
		Mesh* mMesh = nullptr;
		std::map<RenderPass, Material*> mSubMaterials;
	};
}

