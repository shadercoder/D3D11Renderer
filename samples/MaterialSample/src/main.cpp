#include "SampleFramework/SampleFramework.h"
#include "MaterialSample.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#endif

int main(int, char* []) {
	return SampleFramework::Framework::run(
		new MaterialSample{},
		"Clair MaterialSample",
		960, 640,
		"../../common/data/"
	);
}