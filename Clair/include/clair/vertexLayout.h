#pragma once
#include "clair/vertexAttribute.h"
#include <vector>

namespace Clair {
	class VertexLayout {
	public:
		void addAttribute(const VertexAttribute& attribute);

	private:
		friend class Renderer;
		std::vector<VertexAttribute> mAttributes;
	};
}