#pragma once
#include <map>
#include "clair/renderPass.h"
#include "clair/vertexLayout.h"

namespace Clair {
	class VertexLayout;
	class VertexShader;
	class FragmentShader;

	class SubMaterial {
	public:

	private:
		VertexLayout* mVertexLayout;
		VertexShader* mVertexShader;
		FragmentShader* mFragmentShader;
	};
	class Material {
	public:

	private:
		std::map<RenderPass, SubMaterial*> mSubMaterials;

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
	};
}

/* class SubMaterial {
public:
	
private:
	
};
*/