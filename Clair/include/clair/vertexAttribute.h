#pragma once
#include <string>

namespace Clair {
	struct VertexAttribute {
		enum class Format {
			FLOAT1, FLOAT2, FLOAT3, FLOAT4
		};
		std::string name;
		Format format;
	};
}