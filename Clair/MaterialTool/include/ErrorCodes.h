#pragma once
#include <string>

namespace MaterialToolError {
	enum {
		SUCCESS = 0,
		ARGS,
		READ,
		VS,
		REFLECT,
		PS,
		WRITE,

	};

	static const std::string ERROR_STRING[] {
		"Success",
		"Incorrect arguments passed",
		"Couldn't read input file",
		"Vertex shader compilation error",
		"Shader reflection error",
		"Pixel shader compilation error",
		"Couldn't write output file, make sure the path exists"
	};
}