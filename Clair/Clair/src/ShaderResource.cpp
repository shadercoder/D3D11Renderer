#include "Clair/ShaderResource.h"
#include "D3dDevice.h"

using namespace Clair;

ShaderResource::ShaderResource() {
}

ShaderResource::~ShaderResource() {
	terminate();
}

void ShaderResource::initialize(
	ID3D11ShaderResourceView* shaderResource,
	const SubTextureOptions& options) {
	CLAIR_ASSERT(shaderResource, "Error creating render target");
	mD3dShaderResView = shaderResource;
	mOptions = options;
}

void ShaderResource::terminate() {
	if (mD3dShaderResView) {
		mD3dShaderResView->Release();
	}
}
