#include <Windows.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	//if (argc <= 1) return -1;
	for (int i {0}; i < argc; ++i) {
		printf("%d: %s\n", i, argv[i]);
	}
	WIN32_FIND_DATA ffd {};
	HANDLE result {INVALID_HANDLE_VALUE};

	result = FindFirstFile(".\\*", &ffd);
	if (result == INVALID_HANDLE_VALUE) {
		return -1;
	}
	do {
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			printf("%s (dir) - %d %d\n", ffd.cFileName,
				   ffd.ftLastWriteTime.dwHighDateTime,
				   ffd.ftLastWriteTime.dwLowDateTime);
		} else {
			printf("%s (file) - %d %d\n", ffd.cFileName,
				   ffd.ftLastWriteTime.dwHighDateTime,
				   ffd.ftLastWriteTime.dwLowDateTime);
		}
	} while (FindNextFile(result, &ffd) != 0);
	FindClose(result);
	getchar();
	return 0;
}