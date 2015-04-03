#pragma once
#include <map>
#include "clair/renderPass.h"
#include "clair/vertexLayout.h"

namespace Clair {
	class VertexShader;
	class FragmentShader;

	class SubMaterial {
	public:
		VertexLayout vertexLayout;
		VertexShader* vertexShader;
		FragmentShader* fragmentShader;
	};

	class Material {
	public:
		void setSubMaterial(RenderPass pass, SubMaterial* subMaterial);
		SubMaterial* getSubMaterial(RenderPass pass);

	private:
		std::map<RenderPass, SubMaterial*> mSubMaterials;
	};
}

/*
"default:
	shaders:
		vs
		fs
	textures:
		wood_diff
		wood_norm
shadow:
	shaders:
		vs
		fs
 */