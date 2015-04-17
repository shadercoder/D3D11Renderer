#include "SampleFramework/SampleFramework.h"
#include "BasicSample.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#endif

int main(int, char* []) {
	MaterialSample* const sample {new MaterialSample{}};
	if (!SampleFramework::Framework::run(sample, "Clair BasicSample",
										 960, 640,
										 "../../common/data/")) {
		delete sample;
		return -1;
	}
	delete sample;
	return 0;
}