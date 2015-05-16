#pragma once

class Cb_materials_deferred_composite_Ps {
public:
	Clair::Float4x4 InverseProj;
	Clair::Float4x4 View;
	Clair::Float4 LightDiffuseColors[64];
	Clair::Float4 LightPositions[64];
	int32_t DrawGBuffers;
	float __padding0__[3];
};
