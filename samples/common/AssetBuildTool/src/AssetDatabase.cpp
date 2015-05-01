#include "AssetDatabase.h"
#include <fstream>
#include <iostream>
#include <vector>

class File {
public:
	std::string name {};
	std::vector<std::string> dependencies {};
};

static std::ofstream gOutDatabase {};
static std::vector<File> gUpToDateFiles{};
static std::string gWorkingDirectory {};

bool isFiletimeEqual(const FILETIME& a, const FILETIME& b) {
	return a.dwHighDateTime == b.dwHighDateTime
		   && a.dwLowDateTime == b.dwLowDateTime;
}

bool AssetDatabase::initialize(const std::string& workingDirectory) {
	gWorkingDirectory = workingDirectory;
	// open file to read in the database
	const std::string assetBuildFileName {gWorkingDirectory + "/.AssetBuild"};
	std::ifstream inDataBase {assetBuildFileName};
	if (!inDataBase.is_open()) {
		printf(">  Unable to find .AssetBuild file, doing full rebuild\n");
	} else {
		printf(">  Reading .AssetBuild file\n");
	}
	std::string line {};
	int currentDependencyIndex {0};
	int numDependenciesToCheck {0};
	bool isAFileOnHold {false};
	bool isFileOnHoldValid {false};
	File fileToAdd {};
	while (std::getline(inDataBase, line)) {
		std::string name {line.substr(0, line.find_first_of('|'))};
		line = line.substr(line.find_first_of('|') + 1);
		DWORD highTime {std::stoul(line.substr(0, line.find_first_of('|')))};
		line = line.substr(line.find_first_of('|') + 1);
		DWORD lowTime {std::stoul(line.substr(0, line.find_first_of('|')))};
		FILETIME readFileTime {lowTime, highTime};
		auto hasDepens = line.find_first_of('|');
		if (hasDepens != std::string::npos) {
			line = line.substr(hasDepens + 1);
			numDependenciesToCheck = std::stoi(line);
			isFileOnHoldValid = true;
			isAFileOnHold = true;
			fileToAdd = File{};
			fileToAdd.name = name;
		} else {
			if (!isAFileOnHold) {
				numDependenciesToCheck = 0;
				isFileOnHoldValid = true;
				isAFileOnHold = true;
				fileToAdd = File{};
				fileToAdd.name = name;
			}
		}

		if (currentDependencyIndex > 0) {
			std::string relativeName {};
			auto findLastSlashPos = name.find_last_of("/\\");
			if (findLastSlashPos != std::string::npos) {
				relativeName = name.substr(findLastSlashPos + 1);
			}
			fileToAdd.dependencies.push_back(relativeName);
		}
		FILETIME timeFromName {};
		bool isTimeFromNameValid {getFileTimeFromFilename(name, &timeFromName)};
		if (isTimeFromNameValid
			&& isFiletimeEqual(readFileTime, timeFromName)) {
			++currentDependencyIndex;
		} else {
			++currentDependencyIndex;
			isFileOnHoldValid = false;
		}

		if (currentDependencyIndex > numDependenciesToCheck) {
			isAFileOnHold = false;
			currentDependencyIndex = 0;
			if (isFileOnHoldValid) {
				gUpToDateFiles.push_back(fileToAdd);
			}
		}
	}
	inDataBase.close();

	// now make the same file open for writing
	gOutDatabase.open(assetBuildFileName);
	if (!gOutDatabase.is_open()) {
		return false;
	}

	return true;
}

void AssetDatabase::writeFile(const std::string& name, const FILETIME& filetime) {
	gOutDatabase << name << '|'
		 << std::to_string(filetime.dwHighDateTime) << '|'
		 << std::to_string(filetime.dwLowDateTime)
		 << '\n';
}

void AssetDatabase::writeFile(const std::string& name, const FILETIME& filetime,
							  std::vector<std::string>& dependencies) {
	if (dependencies.empty()) {
		writeFile(name, filetime);
		return;
	}
	// dependencies
	gOutDatabase << name << "|"
		 << std::to_string(filetime.dwHighDateTime) << '|'
		 << std::to_string(filetime.dwLowDateTime) << '|'
		 << std::to_string(dependencies.size())
		 << '\n';
	const std::string containingDir {getContainingDirectory(name)};
	for (const auto& it : dependencies) {
		const std::string n {containingDir + it};
		FILETIME t {};
		getFileTimeFromFilename(n, &t);
		writeFile(n, t);
	}
}

bool AssetDatabase::isFileUpToDate(const std::string& name) {
	for (auto& it : gUpToDateFiles) {
		if (it.name == name) {
			FILETIME itTime {};
			getFileTimeFromFilename(it.name, &itTime);
			writeFile(it.name, itTime, it.dependencies);
			//for(const auto& dep : it.dependencies) {
			//	std::cout << "error: " << dep << '\n';
			//}
			return true;
		}
	}
	return false;
}

void AssetDatabase::terminate() {
	gOutDatabase.close();
}

bool AssetDatabase::getFileTimeFromFilename(const std::string& filename,
	FILETIME* outFileTime) {
	FILETIME currentFileTime {};
	HANDLE file2 = CreateFile((gWorkingDirectory + filename).c_str(),
		GENERIC_READ, FILE_SHARE_READ, nullptr,
		OPEN_EXISTING, 0, nullptr);
	if (GetFileTime(file2, nullptr, nullptr, &currentFileTime)) {
		*outFileTime = currentFileTime;
		return true;
	}
	return false;
}

std::string AssetDatabase::getContainingDirectory(const std::string& filename) {
	std::string result {};
	auto findPos = filename.find_last_of("\\/");
	if (findPos != std::string::npos) {
		result = filename.substr(0, findPos) + '/';
	}
	return result;
}