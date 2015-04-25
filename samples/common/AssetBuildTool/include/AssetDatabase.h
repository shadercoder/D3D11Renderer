#pragma once
#include <string>
#include <Windows.h>

class AssetDatabase {
public:
	static bool initialize(const std::string& assetBuildFilename);
	static void writeFile(const std::string& name, const FILETIME& filetime);
	static bool isFileUpToDate(const std::string& name,
							   const FILETIME& filetime);
	static void terminate();
};

