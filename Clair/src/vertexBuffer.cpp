#include "clair/vertexBuffer.h"

using namespace Clair;

void VertexLayout::addElement(const Element& element) {
	mElements.push_back(element);
}