#Clair (D3D11 renderer)
###Description
This is my specialization project as part of my education.

###Demonstration
The following is an excerpt from [Sample.cpp](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/Sample/src/Sample.cpp), with a GIF demonstrating the result of this code.
```C++
CLAIR_RENDER_PASS_ENUM(Pass) {
	DEFAULT,
	NORMALS
};

Clair::Scene* scene {nullptr};

bool Sample::initialize(HWND hwnd) {
	// Initialize Clair using the window's HWND.
	if (!Clair::Renderer::initialize(hwnd)) return false;

	// Load meshes from file and send it to Clair as byte array (char*).
	auto bunnyMeshData = loadBinaryData("../data/models/bunny.cmod");
	auto bunnyMesh = Clair::Renderer::createMesh(bunnyMeshData.data());

	// Load materials from file and send them to Clair as byte array (char*).
	auto defaultMatData = loadBinaryData("../data/materials/default.cmat");
	auto defaultMat = Clair::Renderer::createMaterial(defaultMatData.data());
	auto normalsMatData = loadBinaryData("../data/materials/normals.cmat");
	auto normalsMat = Clair::Renderer::createMaterial(normalsMatData.data());

	// Create a scene and objects with the meshes and materials assigned.
	scene = Clair::Renderer::createScene();
	Clair::Object* bunny {scene->createObject()};
	bunny->setMesh(bunnyMesh);
	bunny->setMatrix(Clair::Matrix(value_ptr(translate(vec3{5.0f, 0.0f, 5.0f}))));
	bunny->setMaterial(CLAIR_RENDER_PASS(Pass::DEFAULT), defaultMat);
	bunny->setMaterial(CLAIR_RENDER_PASS(Pass::NORMALS), normalsMat);

	return true;
}

void Sample::update(const float deltaTime, const float ) {
	// Update camera matrix and current render pass based on input.
	Camera::update(deltaTime);
	if (Input::getKeyDown(SDL_SCANCODE_N)) {
		Clair::Renderer::setRenderPass(CLAIR_RENDER_PASS(Pass::DEFAULT));
	} else if (Input::getKeyDown(SDL_SCANCODE_M)) {
		Clair::Renderer::setRenderPass(CLAIR_RENDER_PASS(Pass::NORMALS));
	}
}

void Sample::render() {
	// Render the scene(s)
	Clair::Renderer::clear();
	Clair::Renderer::setCameraMatrix(
							Clair::Matrix(value_ptr(Camera::getViewMatrix())));
	Clair::Renderer::render(scene);
	Clair::Renderer::finalizeFrame();
}
```

![](http://i.imgur.com/EXTD72A.gif)
