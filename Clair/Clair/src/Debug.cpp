#include <Clair/Debug.h>

using namespace Clair;

LogCallback Log::msCallback;

void Log::setCallback(LogCallback callback) {
	msCallback = callback;
}
