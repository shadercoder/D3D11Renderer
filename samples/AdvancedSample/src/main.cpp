#include "SampleFramework/SampleFramework.h"
#include "AdvancedSample.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#endif

int main(int, char* []) {
	return SampleFramework::Framework::run(
		new AdvancedSample{},
		"Clair AdvancedSample",
		1280, 720,
		"../../common/data/"
	);
}