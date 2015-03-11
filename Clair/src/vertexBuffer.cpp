#include "clair/vertexBuffer.h"

using namespace Clair;

void InputLayoutDesc::addElement(const Element& element) {
	mElements.push_back(element);
}