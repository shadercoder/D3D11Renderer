#####[BasicSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/BasicSample) | DeferredSample | [IBLSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/IBLSample) | [AdvancedSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/AdvancedSample) 

###Introduction
The complete code for this sample can be found in [DeferredSample.cpp](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/DeferredSample/src/DeferredSample.cpp). The samples use the `auto` keyword and often don't use `const` in order to make the code less verbose. Everything explained in the previous samples will not be repeated here.

###Code explanation
#####DeferredSample::createGBufferTarget(format, type)
This helper function creates a render target. A texture (or part of it) can be used as a render target. The `Texture::Options` class is used to set all the options for the texture. These options are passed as an argument to the texture's initialize function.
```C++
Clair::Texture* tex {Clair::ResourceManager::createTexture()};
Clair::Texture::Options texOptions;
texOptions.width = 960;
texOptions.height = 640;
texOptions.format = format;
texOptions.type = type;
tex->initialize(texOptions);
return tex;
```
#####DeferredSample::initialize()
The first thing that happens is the creation of the G-buffer using the previous helper function. The layout of the G-buffer is shown in the comment. The positions will be constructed from the depth buffer and the normals will be encoded as two spherical coordinates (as described [here](http://www.crytek.com/cryengine/cryengine3/presentations/a-bit-more-deferred---cryengine3)). This saves a lot of bandwidth, which is always a good thing for deferred renderers.
```C++
//      |-------------------------------------------|
// RT0: |             depth              | stencil  | 32 bits
//      |-------------------------------------------|
// RT1: |      normal.r       |      normal.g       | 32 bits
//      |-------------------------------------------|
// RT2: | albedo.r | albedo.g | albedo.b | emissive | 32 bits
//      |-------------------------------------------|
mRT0 = createGBufferTarget(
	Clair::Texture::Format::D24_UNORM_S8_UINT,
	Clair::Texture::Type::DEPTH_STENCIL_TARGET);
mRT1 = createGBufferTarget(
	Clair::Texture::Format::R16G16_FLOAT,
	Clair::Texture::Type::RENDER_TARGET);
mRT2 = createGBufferTarget(
	Clair::Texture::Format::R8G8B8A8_UNORM,
	Clair::Texture::Type::RENDER_TARGET);
```
A render target group is created. This class groups one depth/stencil target and multiple color render targets (two in this case).
```C++
mGBuffer = new Clair::RenderTargetGroup{2};
mGBuffer->setDepthStencilTarget(mRT0);
mGBuffer->setRenderTarget(0, mRT1->getRenderTarget());
mGBuffer->setRenderTarget(1, mRT2->getRenderTarget());
```
For deferred rendering, a geometry material is needed to output all the scene information to the G-buffer, and another material is needed to light and composite the final image based on this G-buffer. The geometry material is loaded and initialized the same way as the materials in the previous sample.
```C++
auto geometryMatData = Loader::loadBinaryData("materials/deferred/geometry.cmat");
mGeometryMat = Clair::ResourceManager::createMaterial();
mGeometryMat->initialize(geometryMatData.get());
```
However, the deferred compositing pass is done as a post-process effect. The engine supports post-processing by rendering a fullscreen quad with the supplied material. This material is supplied as a standalone `Clair::MaterialInstance` object (in the last sample these instances were created automatically when a material was assigned to an object).
```C++
auto compositeMatData = Loader::loadBinaryData("materials/deferred/composite.cmat");
Clair::Material* compositeMat {Clair::ResourceManager::createMaterial()};
compositeMat->initialize(compositeMatData.get());
mCompositeMatInstance = Clair::ResourceManager::createMaterialInstance();
mCompositeMatInstance->initialize(compositeMat);
```
To communicate with the shader, a constant buffer is obtained from the material instance using a templated `getConstantBuffer` function call, same as in the previous sample. Note that when the value of `NUM_LIGHTS` (64 in this case) changes, the shader has to be run through the Clair MaterialTool again to regenerate the `.cmat` file and the C++ header (the samples use a custom build step to rebuild assets automatically if they have been changed since the last build).
```C++
// Shader code
cbuffer Buf : register(b1) {
	matrix InverseProj;
	matrix View;
	float4 LightColors[NUM_LIGHTS];
	float4 LightPositions[NUM_LIGHTS];
	bool DrawGBuffers;
};

// Automatically generated C++ header
class Cb_materials_deferred_composite_Ps {
public:
	Clair::Float4x4 InverseProj;
	Clair::Float4x4 View;
	Clair::Float4 LightColors[64];
	Clair::Float4 LightPositions[64];
	int32_t DrawGBuffers;
	float __padding0__[3];
};

// In DeferredSample.cpp: interpret data as the generated class to easily edit constant buffer values
mCompositeCBuffer = mCompositeMatInstance->getConstantBufferPs<Cb_materials_deferred_composite_Ps>();
```
The material does not only take a constant buffer as input, but also the G-buffer textures. The `Clair::MaterialInstance::setShaderResource()` function can be used to bind a `Clair::ShaderResource` to a numbered register. Every texture is by default also a shader resource.
```C++
// Shader code
Texture2D RT0 : register(t0);
Texture2D RT1 : register(t1);
Texture2D RT2 : register(t2);

// In DeferredSample.cpp, the indices correspond with the ones in the shader
mCompositeMatInstance->setShaderResource(0, mRT0->getShaderResource());
mCompositeMatInstance->setShaderResource(1, mRT1->getShaderResource());
mCompositeMatInstance->setShaderResource(2, mRT2->getShaderResource());
```
Besides the texture's default shader resource, you can also make a shader resource from only part of a texture. This sample does not need that functionality, but the AdvancedSample, for example, uses this to filter cube maps (and a screen-space reflection buffer) into higher mips for physically-based rendering.

Then, some objects are placed in the scene and the geometry pass material is assigned to them. This works the same way as in the BasicSample so it is not repeated here.
##### DeferredSample::onResize()
Same as in the previous sample, the swap buffer, viewport and projection matrix have to be adjusted when the window has been resized.
```C++
Clair::Renderer::resizeSwapBuffer(getWidth(), getHeight());
Clair::Renderer::setViewport(0, 0, getWidth(), getHeight());
mProjectionMat = perspectiveLH(radians(90.0f), getAspect(), 0.01f, 100.0f);
Clair::Renderer::setProjectionMatrix(value_ptr(mProjectionMat));
```
In this sample, however, there are also some textures that need to be resized: the textures that make up the G-buffer. In Direct3D you would have to recreate the texture and the objects that use it, but I decided to abstract that away into the `Clair::Texture::resize()` function. This function resizes the texture and updates all objects that point to (part of) it. This means that the texture, the shader resources, the render targets and by extension the render target groups all remain valid.
```C++
mRT0->resize(getWidth(), getHeight());
mRT1->resize(getWidth(), getHeight());
mRT2->resize(getWidth(), getHeight());
```
#####DeferredSample::render()
First, the G-buffer targets are cleared and the scene geometry is rendered to the G-buffer.
```C++
mRT0->clear({1.0f});
mRT1->getRenderTarget()->clear({0.0f, 0.0f, 0.0f, 1.0f});
mRT2->getRenderTarget()->clear({0.0f, 0.0f, 0.0f, 1.0f});
Clair::Renderer::setRenderTargetGroup(mGBuffer);
Clair::Renderer::setViewMatrix(value_ptr(viewMat));
Clair::Renderer::render(mScene);
```
The `Clair::Renderer::setRenderTargetGroup()` function takes as parameter the render target group that the `Clair::Renderer::render()` function should render to. To go back to rendering to the swap chain's back buffer, a `nullptr` is passed to the function, as shown in the code below.

The `Clair::Renderer::renderScreenQuad()` takes as parameter a material instance which will get excecuted on a fullscreen quad. This is used for post-process effects, the deferred compositing pass in this case.
```C++
Clair::Renderer::setRenderTargetGroup(nullptr);
Clair::Renderer::clearDepthStencil(1.0f, 0);
mCompositeCBuffer->InverseProj = value_ptr(inverse(mProjectionMat));
mCompositeCBuffer->View = value_ptr(viewMat);
Clair::Renderer::renderScreenQuad(mCompositeMatInstance);
Clair::Renderer::finalizeFrame();
```
#####Shaders
The code for the material that handles the initial geometry pass is defined in [geometry.hlsl](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/common/rawdata/materials/deferred/geometry.hlsl). This shader renders to multiple render targets (MRT): the G-buffer.
```C
struct PsOut {
	// The depth/stencil target (RT0) is implicit
	float2 RT1 : SV_TARGET0; // Normal is packed as 2 spherical coordinates
	float4 RT2 : SV_TARGET1; // 3 floats for the albedo color and 1 float for the emissive term
};

PsOut psMain(PsIn psIn) {
	PsOut psOut;
	psOut.RT1 = packNormal(normalize(psIn.Normal));
	psOut.RT2 = float4(Albedo, Emissive);
	return psOut;
}
```
The code for the compositing pass is defined in [composite.hlsl](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/common/rawdata/materials/deferred/composite.hlsl). This material takes as input the G-buffer textures and the constant buffer that defines the lights. It uses these input to calculate the lighting per pixel by looping through the lights.
```C
for (int i = 0; i < NUM_LIGHTS; ++i) {
	// Calculate energy-conserving Blinn-Phong lighting for this pixel/light combination
}
```
As the code above shows, this sample loops through the entire list of lights per pixel, which could be optimized by culling lights (for example, using tiled or clustered rendering).

All lighting happens in linear space. This is achieved by transform between gamma space and linear space where appropriate. Right now, the shader takes care of this using `pow(color, 2.2)` to go from gamma to linear and `pow(color, 1 / 2.2)` to go from linear back to gamma. Alternatively, this can be done by telling the graphics API which textures and render targets are in sRGB space, but I think the shader version demonstrates clearly where the transformations happen.

###Final result
![](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/DeferredSample/screenshot.png)

###Credits
See the main [README](https://github.com/TomVeltmeijer/D3D11Renderer#credits).