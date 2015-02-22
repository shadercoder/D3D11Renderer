#include "clair/scene.h"
#include "clair/object.h"

Clair::Scene::Scene()
	: mObjects	(std::vector<Object*>()) {
}

Clair::Scene::~Scene() {
	for (const auto& it : mObjects) delete it;
}

Clair::Object* Clair::Scene::createObject() {
	Object* const newObject = new Object();
	mObjects.push_back(newObject);
	return newObject;
}