#include "SampleFramework/SampleFramework.h"
#include "BasicSample.h"

#ifdef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#endif

int main(int, char* []) {
	BasicSample* const basicSample {new BasicSample{}};
	if (!SampleFramework::Framework::run(basicSample, "Clair basic sample")) {
		delete basicSample;
		return -1;
	}
	delete basicSample;
	return 0;
}