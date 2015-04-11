#include "Material.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <d3d11.h>
#include "Serialization.h"
#include <cassert>

using namespace Clair;

Material::Material(ID3D11Device* const d3dDevice, const char* data) {
	assert(d3dDevice && data);
	mVertexLayout = Serialization::readVertexLayoutFromBytes(data);
	size_t vsSize {0};
	memcpy(&vsSize, data, sizeof(size_t));
	data += sizeof(size_t);
	const char* const vsData = data;
	data += sizeof(char) * vsSize;
	size_t psSize {0};
	memcpy(&psSize, data, sizeof(size_t));
	data += sizeof(size_t);
	const char* const psData = data;
	mVertexShader = new VertexShader{d3dDevice, vsData, vsSize};
	mPixelShader = new PixelShader{d3dDevice, psData, psSize};
}

Material::~Material() {
	delete mVertexShader;
	delete mPixelShader;
}