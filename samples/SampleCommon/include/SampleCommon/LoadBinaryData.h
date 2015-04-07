#pragma once
#include <fstream>
#include <vector>

static std::vector<char> loadBinaryData(const std::string& filename) {
	std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
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
