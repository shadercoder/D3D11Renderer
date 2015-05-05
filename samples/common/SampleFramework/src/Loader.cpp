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
	imgData = stbi_load_from_file(file, &imgx, &imgy, &comp, 0);
	fclose(file);
	Byte* returnData {new Byte[imgx * imgy * comp]};
	for (int x {0}; x < imgx; ++x) {
		for (int y {0}; y < imgy; ++y) {
			const int idx {x + y * imgx};
			const int inverseIdx {x + (y) * imgx};
			returnData[idx * 4  + 0] = imgData[inverseIdx * 4 + 0];
			returnData[idx * 4  + 1] = imgData[inverseIdx * 4 + 1];
			returnData[idx * 4  + 2] = imgData[inverseIdx * 4 + 2];
			//returnData[idx + 0] = imgData[inverseIdx + 0];
		}
	}
	stbi_image_free(imgData);
	return LoadedTexture{imgx, imgy, returnData};
}

LoadedTexture Loader::loadCubeImageData(const std::string& foldername) {
	LoadedTexture textures[6] = { loadImageData(foldername + "/0.png"),
								  loadImageData(foldername + "/1.png"),
								  loadImageData(foldername + "/2.png"),
								  loadImageData(foldername + "/3.png"),
								  loadImageData(foldername + "/4.png"),
								  loadImageData(foldername + "/5.png")};
	const int imgx = textures[0].width;
	const int imgy = textures[0].height;
	Byte* returnData {new Byte[imgx * imgy * 4 * 6]()};
	for (int i = 0; i < 6; ++i) {
		//returnData[img]
		memcpy(&returnData[imgx * imgy * 4 * i],
			   textures[i].data,
			   imgx * imgy * 4);
	}
	return LoadedTexture{imgx, imgy, returnData};
}