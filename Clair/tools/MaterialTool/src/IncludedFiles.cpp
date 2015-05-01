#include "IncludedFiles.h"

void IncludedFiles::addFile(const std::string& file) {
	for (const auto& it : mFiles) {
		if (it == file) {
			return;
		}	
	}
	mFiles.push_back(file);
}

void IncludedFiles::writeToLog(std::ostream& logFile) {
	for (const auto& it : mFiles) {
		logFile << it << '\n';
	}
}