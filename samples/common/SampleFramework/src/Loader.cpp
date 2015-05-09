#include "SampleFramework/Loader.h"
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace SampleFramework;

LoadedTexture::LoadedTexture(const int width,
							 const int height,
							 const Byte* const data) {
	this->width = width;
	this->height = height;
	this->data = data;
}

LoadedTexture::~LoadedTexture() {
	delete[] data;
}

std::string Loader::msSearchPath {};

void Loader::setSearchPath(const std::string& path) {
	msSearchPath = path;
	if (msSearchPath[msSearchPath.length() - 1] != '/') {
		msSearchPath += '/';
	}
}

std::unique_ptr<Byte> Loader::loadBinaryData(const std::string& filename) {
	std::string actualFilename {msSearchPath + filename};
	std::ifstream file(actualFilename.c_str(), std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		printf("Couldn't open file\n");
		return nullptr;
	}
	const auto pos = file.tellg();
	std::vector<char> vec(static_cast<unsigned>(pos));
	file.seekg(0, std::ios::beg);
	file.read(&vec[0], pos);
	std::unique_ptr<Byte> ret{new Byte[vec.size()]};
	memcpy(ret.get(), &vec[0], vec.size());
	return ret;
}

LoadedTexture Loader::loadImageData(const std::string& filename) {
	stbi_uc* imgData {nullptr};
	FILE* file {nullptr};
	fopen_s(&file, (msSearchPath + filename).c_str(), "rb");
	int imgx, imgy, comp;
	imgData = stbi_load_from_file(file, &imgx, &imgy, &comp, 4);
	fclose(file);
	Byte* returnData {new Byte[imgx * imgy * 4]};
	memcpy(returnData, imgData, sizeof(Byte) * imgx * imgy * 4);
	stbi_image_free(imgData);
	return LoadedTexture{imgx, imgy, returnData};
}

LoadedTexture Loader::loadHDRImageData(const std::string& filename) {
	float* imgData {nullptr};
	FILE* file {nullptr};
	fopen_s(&file, (msSearchPath + filename).c_str(), "rb");
	int imgx, imgy, comp;
	imgData = stbi_loadf_from_file(file, &imgx, &imgy, &comp, 4);
	fclose(file);
	float* returnData {new float[imgx * imgy * 4]};
	memcpy(returnData, imgData, sizeof(float) * imgx * imgy * 4);
	stbi_image_free(imgData);
	return LoadedTexture{imgx, imgy, reinterpret_cast<Byte*>(returnData)};
}