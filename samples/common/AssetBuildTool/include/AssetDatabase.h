#pragma once
#include <string>
#include <Windows.h>
#include <vector>

class AssetDatabase {
public:
	static bool initialize(const std::string& assetBuildFilename);
	static void writeFile(const std::string& name, const FILETIME& filetime);
	static void writeFile(const std::string& name, const FILETIME& filetime,
						  std::vector<std::string>& dependencies);
	static bool isFileUpToDate(const std::string& name);
	static void terminate();

private:
	static bool getFileTimeFromFilename(const std::string& filename,
										FILETIME* outFileTime);
	static std::string getContainingDirectory(const std::string& filename);
};

