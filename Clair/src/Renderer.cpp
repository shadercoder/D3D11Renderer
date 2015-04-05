#include <Windows.h>
#include "Clair/Renderer.h"
#include <stdio.h>
#include "Clair/Scene.h"
#include "Clair/Object.h"
#include <vector>
#include "Clair/Matrix.h"
#include "Material.h"
#include "Mesh.h"
#include "Serialization.h"
#include "LowLevelRenderer.h"
#include <cassert>

using namespace Clair;

namespace {
	std::vector<Clair::Material*> materials;
	std::vector<Clair::Mesh*> meshes;
}

bool Clair::Renderer::initialize(HWND hwnd) {
	const bool result {LowLevelRenderer::initialize(hwnd)};
	printf("Clair initialized.\n");
	return result;
}

void Clair::Renderer::terminate() {
	LowLevelRenderer::terminate();
	for (const auto& it : meshes) {
		LowLevelRenderer::destroyVertexBuffer(it->vertexBuffer);
		LowLevelRenderer::destroyIndexBuffer(it->indexBuffer);
		delete it;
	}
	for (const auto& it : materials) {
		LowLevelRenderer::destroyVertexShader(it->vertexShader);
		LowLevelRenderer::destroyPixelShader(it->pixelShader);
		delete it;
	}
	printf("Clair terminated.\n");
}

void Clair::Renderer::clear() {
	LowLevelRenderer::clear();
}

void Clair::Renderer::finalizeFrame() {
	LowLevelRenderer::finalizeFrame();
}

void Clair::Renderer::setViewport(const int x, const int y,
								  const int width, const int height) {
	LowLevelRenderer::setViewport(x, y, width, height);
}

void Clair::Renderer::render(Scene* const scene) {
	if (!scene) return;

	//const XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2,
	//													 viewWidth / viewHeight,
	//													 0.1f, 100.0f);
	//
	//d3dDeviceContext->VSSetShader(vertexShaders[0]->shader, nullptr, 0);
	//d3dDeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	//d3dDeviceContext->PSSetShader(pixelShaders[0]->shader, nullptr, 0);
	//d3dDeviceContext->PSSetShaderResources(0, 1, &shaderResView);
	//d3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
	//ConstantBuffer cb;
	//cb.view = cameraViewMat;
	//cb.projection = projection;
	//
	//for (const auto& it : scene->mObjects) {
	//	const Mesh* const mesh = it->getMesh();
	//	if (!mesh) continue;
	//	cb.world = it->getMatrix();
	//	d3dDeviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb,
	//										0, 0);
	//	const UINT stride = mesh->inputLayout->stride;
	//	const UINT offset = 0;
	//	d3dDeviceContext->IASetInputLayout(mesh->inputLayout->inputLayout);
	//	d3dDeviceContext->IASetVertexBuffers(0, 1, &mesh->vertexBuffer,
	//										 &stride, &offset);
	//	d3dDeviceContext->IASetIndexBuffer(mesh->indexBuffer,
	//									   DXGI_FORMAT_R32_UINT, 0);
	//	d3dDeviceContext->DrawIndexed(mesh->indexBufferSize, 0, 0);
	//}
}

Clair::Scene* Clair::Renderer::createScene() {
	Clair::Scene* const newScene = new Scene();
	//scenes.push_back(newScene);
	delete newScene;
	return newScene;
}

//Clair::InputLayout* Clair::Renderer::createInputLayout(VertexLayoutOld& desc,
//													   VertexShader* const vs) {
//	InputLayout* newInputLayout = new InputLayout;
//	inputLayouts.push_back(newInputLayout);
//	HRESULT result;
//	std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;
//	unsigned stride = 0;
//	for (const auto& it : desc.mElements) {
//		auto format = DXGI_FORMAT_R32_FLOAT;
//		switch (it.format) {
//		case VertexLayoutOld::Element::Format::FLOAT2: stride += sizeof(float) * 2;
//			format = DXGI_FORMAT_R32G32_FLOAT; break;
//		case VertexLayoutOld::Element::Format::FLOAT3: stride += sizeof(float) * 3;
//			format = DXGI_FORMAT_R32G32B32_FLOAT; break;
//		case VertexLayoutOld::Element::Format::FLOAT4: stride += sizeof(float) * 4;
//			format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
//		}
//		layoutDesc.push_back({it.name.c_str(), 0, format, 0, it.offset,
//							  D3D11_INPUT_PER_VERTEX_DATA, 0});
//	}
//	result = d3dDevice->CreateInputLayout(layoutDesc.data(), layoutDesc.size(),
//										  vs->byteCode, vs->byteCodeSize,
//										  &newInputLayout->inputLayout);
//	if (FAILED(result)) return nullptr;
//	newInputLayout->stride = stride;
//	
//	return newInputLayout;
//}


Mesh* Renderer::createMesh(char* data) {
	assert(data);
	VertexLayout vertexLayout {Serialization::readVertexLayoutFromBytes(data)};
	unsigned stride {0};
	memcpy(&stride, data, sizeof(unsigned));
	data += sizeof(unsigned);
	unsigned numVertices {0};
	memcpy(&numVertices, data ,sizeof(unsigned));
	data += sizeof(unsigned);
	char* const vertexData {new char[numVertices * stride]};
	memcpy(vertexData, data, sizeof(char) * numVertices * stride);
	data += sizeof(char) * numVertices * stride;
	unsigned numIndices {0};
	memcpy(&numIndices, data, sizeof(unsigned));
	data += sizeof(unsigned);
	unsigned* const indexData {new unsigned[numIndices]};
	memcpy(indexData, data, sizeof(unsigned) * numIndices);

	Mesh* const mesh {new Mesh{}};
	mesh->vertexLayout = vertexLayout;
	mesh->vertexBuffer = LowLevelRenderer::
		createVertexBuffer(vertexData, numVertices * stride);
	mesh->indexBuffer = LowLevelRenderer::
		createIndexBuffer(indexData, numIndices);
	mesh->indexBufferSize = numIndices;
	meshes.push_back(mesh);
	return mesh;
}

Material* Renderer::createMaterial(char* data) {
	assert(data);
	const VertexLayout vertexLayout {
		Serialization::readVertexLayoutFromBytes(data)};
	size_t vsSize {0};
	memcpy(&vsSize, data, sizeof(size_t));
	data += sizeof(size_t);
	char* const vsData = data;
	data += sizeof(char) * vsSize;
	size_t psSize {0};
	memcpy(&psSize, data, sizeof(size_t));
	data += sizeof(size_t);
	char* const psData = data;

	Material* const material {new Material{}};
	material->vertexLayout = vertexLayout;
	material->vertexShader = LowLevelRenderer::
								createVertexShader(vsData, vsSize);
	material->pixelShader = LowLevelRenderer::
								createPixelShader(psData, psSize);
	materials.push_back(material);
	return material;
}

void Clair::Renderer::setCameraMatrix(const Clair::Matrix&) {
	//cameraViewMat = m;
	return;
}
