#include "SampleFramework/SampleFramework.h"
#include "BasicSample.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#endif

int main(int, char* []) {
	return SampleFramework::Framework::run(
		new BasicSample{},
		"Clair BasicSample",
		960, 640,
		"../../common/data/"
	);
}