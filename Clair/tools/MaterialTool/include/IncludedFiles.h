#pragma once
#include <string>
#include <vector>

class IncludedFiles {
public:
	void addFile(const std::string& file);
	void writeToLog(std::ostream& logFile);

private:
	std::vector<std::string> mFiles;
};