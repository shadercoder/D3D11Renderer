#pragma once
#include <string>

namespace MeshToolError {
	enum {
		SUCCESS = 0,
		ARGS,
		READ,
		WRITE,
	};

	static const std::string ERROR_STRING[] {
		"Success",
		"Incorrect arguments passed",
		"Couldn't read input file",
		"Couldn't write output file, make sure the path exists"
	};
}