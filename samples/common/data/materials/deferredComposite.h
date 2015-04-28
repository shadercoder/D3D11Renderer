#pragma once

class Cb_materials_deferredComposite_Ps {
public:
	Clair::Float4 LightDiffuseColors[128];
	Clair::Float4 LightPositions[128];
	int32_t DrawGBuffers;
	float __padding0__[3];
};
