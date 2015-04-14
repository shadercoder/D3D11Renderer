#include "Mesh.h"
#include <d3d11.h>
#include "Serialization.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Clair/Debug.h"

using namespace Clair;

Mesh::Mesh(const char* data) {
	CLAIR_ASSERT(data, "Mesh data is null");
	mVertexLayout = Serialization::readVertexLayoutFromBytes(data);
	unsigned stride {0};
	memcpy(&stride, data, sizeof(unsigned));
	data += sizeof(unsigned);
	unsigned numVertices {0};
	memcpy(&numVertices, data ,sizeof(unsigned));
	data += sizeof(unsigned);
	const char* const vertexData {data};
	data += sizeof(char) * numVertices * stride;
	unsigned numIndices {0};
	memcpy(&numIndices, data, sizeof(unsigned));
	data += sizeof(unsigned);
	const unsigned* const indexData {reinterpret_cast<const unsigned*>(data)};

	mVertexBuffer = new VertexBuffer {vertexData, numVertices * stride};
	mIndexBuffer = new IndexBuffer {indexData, numIndices * sizeof(unsigned)};
	mIndexBufferSize = numIndices;
}

Mesh::~Mesh() {
	delete mIndexBuffer;
	delete mVertexBuffer;
}
