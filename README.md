<img src="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/banner.png" width="100%" height="auto">
#Clair (D3D11 renderer)
###Description
This is my specialization project as part of my education.

###Samples
[BasicSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/BasicSample) | [DeferredSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/DeferredSample)
:--|:--
Draws a mesh with a simple animated material. | Uses features such as RTT, MRT and post-processing to do basic deferred shading.
<a href="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/BasicSample"><img src="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/BasicSample/screenshot.png"  width="100%" height="auto"></a> | <a href="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/DeferredSample"><img src="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/DeferredSample/screenshot.png" width="100%" height="auto"></a>

[IBLSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/IBLSample) | [AdvancedSample](https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/AdvancedSample)
:--|:--
Uses an HDR image to create a pre-filtered cube map to do image-based lighting with. | Combines techniques from all previous samples and adds some extra effects such as SSR.
<a href="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/IBLSample"><img src="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/IBLSample/screenshot.png" width="100%" height="auto"></a> | <a href="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/AdvancedSample"><img src="https://github.com/TomVeltmeijer/D3D11Renderer/blob/master/samples/AdvancedSample/screenshot.png" width="100%" height="auto"></a>

###Credits
####Code
Libraries:

- [Assimp](http://assimp.sourceforge.net/)

- [GLM](https://github.com/g-truc/glm)

- [SDL2](https://www.libsdl.org/index.php)

- [stb_image.h](https://github.com/nothings/stb)

- [ImGui](https://github.com/ocornut/imgui)

####Assets
Models/textures:

- ["Stanford Bunny"](http://graphics.stanford.edu/~mdfisher/Data/Meshes/bunny.obj)

- ["Cerberus" - Andrew Maximov](http://artisaverb.info/Cerberus.html)

HDR probes:

- http://www.hdrlabs.com/sibl/archive.html

- http://gl.ict.usc.edu/Data/HighResProbes
