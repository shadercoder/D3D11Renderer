#pragma once

class Cb_materials_advanced_composite_Ps {
public:
	Clair::Float4x4 InverseView;
	Clair::Float4x4 InverseProj;
	Clair::Float4x4 View;
	Clair::Float4 LightColors[32];
	Clair::Float4 LightPositions[32];
	float Ambient;
	int32_t SSREnabled;
	int32_t LightsEnabled;
	float __padding0__[1];
};
