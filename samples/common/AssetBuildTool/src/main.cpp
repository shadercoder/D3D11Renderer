#include <Windows.h>
#include <stdio.h>
#include <string>
#include <fstream>

static std::string gInFile {"../../rawdata"};
static std::string gOutFile {"../../data"};
static int gNumSucceeded {0};
static int gNumFailed {0};
static int gNumUpToDate {0};
static int gIndentLevel {0};
std::ofstream gTimeStampFile {};

static bool scanFolder(const std::string& folder);

int main(int argc, char* argv[]) {
	// Get paths from command arguments (or hardcoded values for debugging)
	if (argc != 3) {
		return -1;
	} else {
		gInFile = argv[1];
		gOutFile = argv[2];
	}
	printf("------ AssetBuildTool started ------\n");

	// Check for timestamp file
	const std::string timeStampFileName {gInFile + "\\.AssetBuild"};
	std::ifstream timeStampFileCheck {timeStampFileName};
	if (!timeStampFileCheck.is_open()) {
		printf(">  Unable to find .AssetBuild file, doing full rebuild\n");
	} else {
		timeStampFileCheck.close();
		printf(">  Reading .AssetBuild file\n");
	}
	gTimeStampFile.open(timeStampFileName);
	if (!gTimeStampFile.is_open()) {
		return -1;
	}
	printf(">  Converting files:\n");

	// Scan files
	if (!scanFolder("")) {
		return -1;
	}

	// Finish
	gTimeStampFile.close();
	printf("========== AssetBuildTool: "
		   "%u succeeded, %u failed, %u up-to-date ==========\n",
		   gNumSucceeded, gNumFailed, gNumUpToDate);
	return 0;
}

static bool scanFolder(const std::string& folder) {
	WIN32_FIND_DATA ffd {};
	HANDLE hFile {INVALID_HANDLE_VALUE};
	const std::string currFolder {gInFile + folder + ".\\*"};
	hFile = FindFirstFile(currFolder.c_str(), &ffd);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	++gIndentLevel;
	std::string indent {"|"};
	if (gIndentLevel == 1) {
		indent += "-";	
	} else {
		for (int i {0}; i < gIndentLevel - 1; ++i) {
			indent += "  ";
		}
		indent += "|-";
	}
	do {
		const std::string currFile {ffd.cFileName};
		if (currFile == "." || currFile == "..") {
			continue;
		}
		const auto subPos = currFile.find_last_of(".");
		if (subPos != std::string::npos && currFile.substr(subPos) == ".hlsl") {
			printf(">  %s %s\n", indent.c_str(), ffd.cFileName);
			std::string currOutFile {currFile};
			currOutFile = currOutFile.substr(0, currOutFile.length() - 4);
			currOutFile += "cmat";
			gTimeStampFile << folder << '/' + currFile << "|"
				 << std::to_string(ffd.ftLastWriteTime.dwHighDateTime) << "|"
				 << std::to_string(ffd.ftLastWriteTime.dwLowDateTime)
				 << '\n';
			std::string command {"..\\..\\Clair\\MaterialTool\\bin\\"
									   "MaterialTool_d.exe "};
			command += gInFile + folder + currFile + " ";
			command += gOutFile + folder + currOutFile + " -s";
			std::system(command.c_str());
			++gNumSucceeded;
		}
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			printf(">  %s %s\n", indent.c_str(), ffd.cFileName);
			//printf(">  %s %s (dir) - %d %d\n", indent.c_str(), ffd.cFileName,
			//	   ffd.ftLastWriteTime.dwHighDateTime,
			//	   ffd.ftLastWriteTime.dwLowDateTime);
			if (!scanFolder(folder + "/" + currFile)) {
				++gNumFailed;
			}
		}
	} while (FindNextFile(hFile, &ffd) != 0);
	FindClose(hFile);
	--gIndentLevel;
	return true;
}