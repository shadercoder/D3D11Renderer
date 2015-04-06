#Clair (D3D11 renderer)
This is my specialization project as part of my education.

##Demonstration
The following is an excerpt from [Sample.cpp](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/Sample/src/Sample.cpp), with a GIF demonstrating the result of this code.
```C++
bool Sample::initialize(HWND hwnd) {
	// Initialize Clair using the window's HWND.
	if (!Clair::Renderer::initialize(hwnd)) return false;

	// Load mesh from file and send it to Clair as byte array (char*).
	auto meshData = loadBinaryData("../data/model.cmod");
	auto mesh = Clair::Renderer::createMesh(meshData.data());

	// Load materials from file and send them to Clair as byte array (char*).
	auto defaultMatData = loadBinaryData("../data/default.cmat");
	auto defaultMat = Clair::Renderer::createMaterial(defaultMatData.data());
	auto normalsMatData = loadBinaryData("../data/normals.cmat");
	auto normalsMat = Clair::Renderer::createMaterial(normalsMatData.data());

	// Create a scene and a object with the mesh and materials assigned.
	scene = Clair::Renderer::createScene();
	Clair::Object* obj {scene->createObject()};
	obj->setMesh(mesh);
	// Different materials can be assigned to different render passes.
	obj->setMaterial(Clair::RenderPass::DEFAULT, defaultMat);
	obj->setMaterial(Clair::RenderPass::NORMALS_ONLY, normalsMat);
	obj->setMatrix(Clair::Matrix(value_ptr(mat4{})));

	return true;
}

void Sample::update(const float deltaTime, const float ) {
    // Update camera matrix and current render pass.
	Camera::update(deltaTime);
	if (Input::getKeyDown(SDL_SCANCODE_N)) {
		Clair::Renderer::setRenderPass(Clair::RenderPass::DEFAULT);
	} else if (Input::getKeyDown(SDL_SCANCODE_M)) {
		Clair::Renderer::setRenderPass(Clair::RenderPass::NORMALS_ONLY);
	}
}

void Sample::render() {
    // Render the scene.
	Clair::Renderer::clear();
	Clair::Renderer::setCameraMatrix(
							Clair::Matrix(value_ptr(Camera::getViewMatrix())));
	Clair::Renderer::render(scene);
	Clair::Renderer::finalizeFrame();
}
```

![](http://i.imgur.com/EXTD72A.gif)
