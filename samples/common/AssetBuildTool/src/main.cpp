#include <Windows.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include "../../../../Clair/tools/MaterialTool/include/ErrorCodes.h"
#include "../../../../Clair/tools/MeshTool/include/ErrorCodes.h"
#include <algorithm>

static std::string gInFile {"../../rawdata"};
static std::string gOutFile {"../../data"};
static int gNumSucceeded {0};
static int gNumFailed {0};
static int gNumUpToDate {0};
static std::ofstream gTimeStampFile {};
static int gIndentLevel {0};
static std::string gIndent {""};

static bool scanFolder(const std::string& folder);
static void processFile(const std::string& currFile, const std::string& folder,
						const WIN32_FIND_DATA& ffd);
static void updateIndent();
static std::string pathToName(const std::string& str);

int main(int argc, char* argv[]) {
	// Get paths from command arguments (or hardcoded values for debugging)
	if (argc != 3) {
		return -1;
	} else {
		gInFile = argv[1];
		gOutFile = argv[2];
	}
	printf("\n");
	printf(">------ AssetBuildTool started ------\n");

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
		   "%i succeeded, %i failed, %i up-to-date ==========\n",
		   gNumSucceeded, gNumFailed, gNumUpToDate);
	if (gNumFailed > 0) {
		const char* const fileword {gNumFailed == 1 ? "file" : "files"};
		printf("ERROR: AssetBuildTool: %i %s failed to convert, "
			   "see build output.\n", gNumFailed, fileword);
		printf("\n");
		return -1;
	}
	printf("\n");
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
	do {
		updateIndent();
		const std::string currFile {ffd.cFileName};
		if (currFile == "." || currFile == "..") {
			continue;
		}
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			printf(">  %s %s\n", gIndent.c_str(), ffd.cFileName);
			std::string subPath {folder + (gIndentLevel > 0 ? "/" : "")
								 + currFile};
			if (!scanFolder(subPath)) {
				++gNumFailed;
			}
		} else {
			processFile(currFile, folder, ffd);
		}
	} while (FindNextFile(hFile, &ffd) != 0);
	FindClose(hFile);
	--gIndentLevel;
	return true;
}

void processFile(const std::string& currFile, const std::string& folder,
				 const WIN32_FIND_DATA& ffd) {
	// Check file type by extension
	const auto subPos = currFile.find_last_of(".");
	if (subPos == std::string::npos) {
		return;
	}
	const std::string extension {currFile.substr(subPos)};
	bool isMesh {false};
	if (extension == ".obj") {
		isMesh = true;
	} else if (extension == ".hlsl") {
		isMesh = false;
	} else {
		return;
	}
	std::string currOutFile {currFile};
	if (isMesh) {
		currOutFile = currOutFile.substr(0, currOutFile.length() - 3);
		currOutFile += "cmod";
	} else {
		currOutFile = currOutFile.substr(0, currOutFile.length() - 5);
		//currOutFile += "cmat";
	}

	// Run the correct command line tool
	std::string command {};
	if (isMesh) {
		command = std::string{"..\\..\\Clair\\tools\\MeshTool\\bin\\"
		#ifdef NDEBUG
							 "MeshTool.exe "};
		#else
							 "MeshTool_d.exe "};
		#endif
		command += gInFile + folder + "/" + currFile + " ";
		command += gOutFile + folder + "/" + currOutFile + " -sb";
	} else {
		command = std::string{"..\\..\\Clair\\tools\\MaterialTool\\bin\\"
		#ifdef NDEBUG
							 "MaterialTool.exe "};
		#else
							 "MaterialTool_d.exe "};
		#endif
		command += gInFile + folder + "/" + currFile + " ";
		command += gOutFile + folder + "/" + currOutFile + ' ' +
				   pathToName(folder + "/" + currFile) + " -s";
	}
	const int commandResult {std::system(command.c_str())};

	// Check if it was successful
	if (commandResult == 0) {
		printf(">  %s %s -> SUCCES\n", gIndent.c_str(), ffd.cFileName,
								   currOutFile.c_str());
		gTimeStampFile << folder << '/' + currFile << "|"
			 << std::to_string(ffd.ftLastWriteTime.dwHighDateTime) << "|"
			 << std::to_string(ffd.ftLastWriteTime.dwLowDateTime)
			 << '\n';
		++gNumSucceeded;
	} else {
		if (isMesh) {
			printf(">  %s %s -> FAILED (MeshTool: %s)\n", gIndent.c_str(),
				   ffd.cFileName,
				   MeshToolError::ERROR_STRING[commandResult].c_str());
		} else {
			printf(">  %s %s -> FAILED (MaterialTool: %s)\n", gIndent.c_str(),
				   ffd.cFileName,
				   MaterialToolError::ERROR_STRING[commandResult].c_str());
		}
		++gNumFailed;
	}
}

static void updateIndent() {
	gIndent = "|";
	if (gIndentLevel == 1) {
		gIndent += "-";	
	} else {
		for (int i {0}; i < gIndentLevel - 1; ++i) {
			gIndent += "  ";
		}
		gIndent += "|-";
	}
}

static std::string pathToName(const std::string& str) {
	std::string r = str.substr(1);
	auto pos = r.find_last_of(".");
	if (pos != std::string::npos) {
		r = r.substr(0, pos);
	}
	std::replace(r.begin(), r.end(), '/', '_');
	r.erase(std::remove(r.begin(), r.end(), ' '), r.end());
	return r;
}