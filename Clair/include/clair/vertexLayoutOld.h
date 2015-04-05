#pragma once
#include <vector>

/*
struct VertexAttribute {
};
class VertexLayout {
	std::vector<VertexAttribute> mAttributes;
};
*/

namespace Clair {
	class VertexLayoutOld {
	public:
		class Element {
		public:
			enum class Format {
				FLOAT1, FLOAT2, FLOAT3, FLOAT4
			};
			std::string name;
			Format format;
			unsigned offset;
		};

		void addElement(const Element& element);

	private:
		friend class Renderer;
		std::vector<Element> mElements;
	};

	class InputLayout;
	struct MeshDesc {
		InputLayout* inputLayout;
		void* vertexData;
		size_t vertexDataSize;
		unsigned* indexData;
		size_t numIndices;
	};
}