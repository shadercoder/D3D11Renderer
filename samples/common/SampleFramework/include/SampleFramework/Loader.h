#pragma once
#include <string>
#include <vector>
#include <memory>

namespace SampleFramework {
	class Loader {
	public:
		static void setSearchPath(const std::string& path);
		static std::vector<char> loadBinaryData(const std::string& filename);
		static std::unique_ptr<char> loadImageData(const std::string& filename);

	private:
		static std::string msSearchPath;
	};
}