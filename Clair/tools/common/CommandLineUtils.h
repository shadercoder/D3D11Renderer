#pragma once
#include <string>
#include <vector>

class CommandLineUtils {
public:
	static std::vector<char> getCommands(int argc, char* argv[]) {
		std::vector<char> result {};
		for (int i {0}; i < argc; ++i) {
			std::string str {argv[i]};
			if (str.length() < 2 || str[0] != '-') continue;
			str = str.substr(1);
			for (size_t c {0}; c < str.length(); ++c) {
				result.push_back(str[c]);
			}
		}
		return result;
	}
};