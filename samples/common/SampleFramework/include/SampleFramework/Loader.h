#pragma once
#include <string>
#include <vector>
#include <memory>

typedef unsigned char Byte;

namespace SampleFramework {
	class LoadedTexture {
	public:
		LoadedTexture(int width, int height, const Byte* data);
		~LoadedTexture();

		int width {0};
		int height {0};
		const Byte* data {nullptr};
	};
	class Loader {
	public:
		static void setSearchPath(const std::string& path);
		static std::unique_ptr<Byte> loadBinaryData(const std::string& filename);
		static LoadedTexture loadImageData(const std::string& filename);
		static LoadedTexture loadHDRImageData(const std::string& filename);
		static LoadedTexture loadCubeImageData(const std::string& foldername);

	private:
		static std::string msSearchPath;
	};
}