#####BasicSample | [DeferredSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/DeferredSample) | [IBLSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/IBLSample) | [AdvancedSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/AdvancedSample) 

###Introduction
The complete code for this sample can be found in [BasicSample.cpp](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/BasicSample/src/BasicSample.cpp). The samples use the `auto` keyword and often don't use `const` in order to make the code less verbose.

###Code explanation
#####BasicSample::initialize()
First, the engine has to be initialized by providing the window's HWND and a user-defined callback function for logging.
```C++
if (!Clair::initialize(hwnd, Logger::log)) {
  return false;
}
```
A mesh of the engine's *cmod* format is loaded by the user and the byte array is used to create a mesh.
```C++
auto bunnyMeshData = Loader::loadBinaryData("models/bunny.cmod");
Clair::Mesh* bunnyMesh {Clair::ResourceManager::createMesh()};
bunnyMesh->initialize(bunnyMeshData.get());
```
A material is created in a similar manner from a *cmat* file.
```C++
auto defaultMatData = Loader::loadBinaryData("materials/basic.cmat");
Clair::Material* defaultMat = Clair::ResourceManager::createMaterial();
defaultMat->initialize(defaultMatData.get());
```
Then, a scene with an object are created. A mesh and a transformation matrix are assigned to the object. 
```C++
mScene = Clair::ResourceManager::createScene();
mBunny = mScene->createObject();
mBunny->setMesh(bunnyMesh);
mBunny->setMatrix(glm::value_ptr(glm::translate(glm::vec3{0.0f})));
```
The material is also assigned to the object, which creates a material instance. The render pass parameter is to easily switch materials of many objects (for example, when doing a shadow map pass). Each object can have one material per render pass. This sample does not use this functionality so the default (0) is used.
```C++
Clair::MaterialInstance* matInstance {mBunny->setMaterial(CLAIR_RENDER_PASS(0), material)};
```
To change shader variables, a constant buffer is used. When the command line material tool finds a constant buffer in the shader in register b1, it will use this as the constant buffer for this material. The tool outputs a header that has the same names and layout as the buffer in the shader code. This header can be included to easily edit the material properties.
```C++
// Constant buffer in shader code
cbuffer Material : register(b1) {
	float4 DiffuseColor;
	float Time;
}

// Automatically generated header with equivalent types and padding where necessary
class Cb_materials_basic_Ps {
public:
	Clair::Float4 DiffuseColor;
	float Time;
	float __padding0__[3];
};
```
An instance of the constant buffer for the material instance is obtained with the getConstantBuffer function. This function takes the constant buffer class as a templated argument to correctly allocate and interpret the data. The values can then be easily set by assignment. The templated `Clair::Matrix` class supports initializer list initialization to make the client code clean and easy to read.
```C++
mConstBuffer = matInstance->getConstantBufferPs<Cb_materials_basic_Ps>();
mConstBuffer->DiffuseColor = {0.8f, 0.2f, 0.1f, 1.0f};
mConstBuffer->Time = getRunningTime();
```
#####void BasicSample::onResize()
When the window has been resized, the swap buffer and viewport should be resized as well. This is done with the following calls. The projection matrix is updated here too in case the aspect ratio of the viewport has changed.
```C++
Clair::Renderer::resizeSwapBuffer(getWidth(), getHeight());
Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
Clair::Renderer::setProjectionMatrix(value_ptr(perspectiveLH(radians(90.0f), getAspect(), 0.01f, 100.0f)));
```
#####void BasicSample::render()
Finally, the back buffer is cleared, the camera's view matrix is set and the scene is rendered. The call to finalizeFrame() presents the buffer to the user.
```C++
Clair::Renderer::clearColor({0.2f, 0.4f, 0.6f, 1.0f});
Clair::Renderer::clearDepthStencil(1.0f, 0);
Clair::Renderer::setViewMatrix(value_ptr(Camera::getViewMatrix()));
Clair::Renderer::render(mScene);
Clair::Renderer::finalizeFrame();
```

###Final result
![](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/BasicSample/screenshot.png)

###Credits
See the main [README](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/README.md).