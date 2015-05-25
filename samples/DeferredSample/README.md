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
	float4 LightDiffuseColors[NUM_LIGHTS];
	float4 LightPositions[NUM_LIGHTS];
	bool DrawGBuffers;
};

// Automatically generated C++ header
class Cb_materials_deferred_composite_Ps {
public:
	Clair::Float4x4 InverseProj;
	Clair::Float4x4 View;
	Clair::Float4 LightDiffuseColors[64];
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

###Final result
![](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/DeferredSample/screenshot.png)

###Credits
See the main [README](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/README.md).