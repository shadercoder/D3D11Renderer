#pragma once
#include <string>
#include <vector>
#include <memory>

typedef unsigned char Byte;

namespace SampleFramework {
	class Loader {
	public:
		static void setSearchPath(const std::string& path);
		static std::unique_ptr<Byte> loadBinaryData(const std::string& filename);
		static std::unique_ptr<Byte> loadImageData(const std::string& filename);

	private:
		static std::string msSearchPath;
	};
}