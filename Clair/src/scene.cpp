#include "clair/scene.h"
#include "clair/object.h"
#include "clair/camera.h"

Clair::Scene::Scene()
	: mObjects	(std::vector<Object*>())
	, mCameras	(std::vector<Camera*>()) {
}

Clair::Scene::~Scene() {
	for (const auto& it : mObjects) delete it;
	for (const auto& it : mCameras) delete it;
}

Clair::Object* Clair::Scene::createObject() {
	Object* const newObject = new Object();
	mObjects.push_back(newObject);
	return newObject;
}

Clair::Camera* Clair::Scene::createCamera() {
	Camera* const newCamera = new Camera();
	mCameras.push_back(newCamera);
	return newCamera;
}