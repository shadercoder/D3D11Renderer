#include "Clair/MaterialInstance.h"

using namespace Clair;

MaterialInstance::~MaterialInstance() {
	if (mConstantBufferData) {
		delete mConstantBufferData;
	}
}
