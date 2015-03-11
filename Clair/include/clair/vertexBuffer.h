#pragma once
#include <string>
#include <vector>

namespace Clair {
	struct InputLayoutDesc {
		class Element {
		public:
			enum class Format {
				float1, FLOAT2, FLOAT3, FLOAT4
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

	//struct VertexBufferDesc {
	//	void* vertexData;
	//	size_t vertexDataSize;
	//	unsigned* indexData;
	//	size_t indexDataSize;
	//};

	class InputLayout;
	struct MeshDesc {
		InputLayout* inputLayout;
		void* vertexData;
		size_t vertexDataSize;
		unsigned* indexData;
		size_t numIndices;
	};
}