#pragma once
#include "Clair/Matrix.h"
#include "Clair/RenderPass.h"
#include <map>

namespace Clair {
	class Mesh;
	class Material;
	class InputLayout;

	class Node {
	public:
		Node() : mMatrix(Matrix()) {}
		virtual ~Node() {}

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
		~Object();

		void setMesh(Mesh* mesh);
		const Mesh* getMesh() const { return mMesh; }

		void setMaterial(RenderPass pass, Material* material);
		Material* getMaterial(RenderPass pass);

		InputLayout* getInputLayout() const;

	private:
		class MaterialInstance {
		public:
			bool isValid {false};
			Material* material {nullptr};
		};
		void recreateInputLayout(MaterialInstance* materialInstance);

		Mesh* mMesh {nullptr};
		std::map<RenderPass, MaterialInstance*> mMatInstances {};
		InputLayout* mInputLayout {};
	};

	inline InputLayout* Object::getInputLayout() const {
		return mInputLayout;
	}
}

