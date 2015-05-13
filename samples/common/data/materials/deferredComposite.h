#pragma once

class Cb_materials_deferredComposite_Ps {
public:
	Clair::Float4x4 InverseViewProj;
	Clair::Float4 LightDiffuseColors[64];
	Clair::Float4 LightPositions[64];
	Clair::Float3 CameraPosition;
	int32_t DrawGBuffers;
};
