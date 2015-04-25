#include "AssetDatabase.h"
#include <fstream>
#include <map>

static std::map<std::string, FILETIME> gDatabase {};
static std::ofstream gOutDatabase {};

bool isFiletimeEqual(const FILETIME& a, const FILETIME& b) {
	return a.dwHighDateTime == b.dwHighDateTime
		   && a.dwLowDateTime == b.dwLowDateTime;
}

bool AssetDatabase::initialize(const std::string& assetBuildFilename) {
	// open file to read in the database
	std::ifstream inDataBase {assetBuildFilename};
	if (!inDataBase.is_open()) {
		printf(">  Unable to find .AssetBuild file, doing full rebuild\n");
	} else {
		printf(">  Reading .AssetBuild file\n");
	}
	std::string line {};
	while (std::getline(inDataBase, line)) {
		std::string name {line.substr(0, line.find_first_of('|'))};
		line = line.substr(line.find_first_of('|') + 1);
		DWORD highTime {std::stoul(line.substr(0, line.find_first_of('|')))};
		line = line.substr(line.find_first_of('|') + 1);
		DWORD lowTime {std::stoul(line.substr(0, line.find_first_of('|')))};
		FILETIME filetime {lowTime, highTime};
		gDatabase[name] = filetime;
	}
	inDataBase.close();

	// now make the same file open for writing
	gOutDatabase.open(assetBuildFilename);
	if (!gOutDatabase.is_open()) {
		return false;
	}

	return true;
}

void AssetDatabase::writeFile(const std::string& name, const FILETIME& filetime) {
	gOutDatabase << name << "|"
		 << std::to_string(filetime.dwHighDateTime) << "|"
		 << std::to_string(filetime.dwLowDateTime)
		 << '\n';
}

bool AssetDatabase::isFileUpToDate(const std::string& name, const FILETIME& filetime) {
	if (gDatabase.count(name) == 0) {
		return false;
	}
	return isFiletimeEqual(gDatabase[name], filetime);
}

void AssetDatabase::terminate() {
	gOutDatabase.close();
}