#include "Clair/ShaderResource.h"
#include "D3dDevice.h"

using namespace Clair;

ShaderResource::~ShaderResource() {
	if (mD3dShaderResView) {
		mD3dShaderResView->Release();
	}
}
