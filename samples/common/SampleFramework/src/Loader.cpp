#include "SampleFramework/Loader.h"
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace SampleFramework;

std::string Loader::msSearchPath {};

void Loader::setSearchPath(const std::string& path) {
	msSearchPath = path;
	if (msSearchPath[msSearchPath.length() - 1] != '/') {
		msSearchPath += '/';
	}
}

std::vector<char> Loader::loadBinaryData(const std::string& filename) {
	std::string actualFilename {msSearchPath + filename};
	std::ifstream file(actualFilename.c_str(), std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		printf("Couldn't open file\n");
		return std::vector<char>();
	}
	const auto pos = file.tellg();
	std::vector<char> vec(static_cast<unsigned>(pos));
	file.seekg(0, std::ios::beg);
	file.read(&vec[0], pos);
	return vec;
}

std::unique_ptr<char> Loader::loadImageData(const std::string& filename) {
	stbi_uc* imgData {nullptr};
	FILE* file {nullptr};
	fopen_s(&file, (msSearchPath + filename).c_str(), "rb");
	int imgx, imgy, comp;
	imgData = stbi_load_from_file(file, &imgx, &imgy, &comp, 0);
	fclose(file);
	char* returnData = new char[imgx * imgy * comp];
	memcpy(returnData, imgData, imgx * imgy * comp);
	stbi_image_free(imgData);
	return std::unique_ptr<char>(returnData);
}