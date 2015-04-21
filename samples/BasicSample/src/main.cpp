#include "SampleFramework/SampleFramework.h"
#include "BasicSample.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#endif

int main(int, char* []) {
	SampleFramework::SampleBase* const sample {new BasicSample{}};
	if (!SampleFramework::Framework::run(sample, "Clair BasicSample",
										 960, 640,
										 "../../common/data/")) {
		delete sample;
		return -1;
	}
	delete sample;
	return 0;
}