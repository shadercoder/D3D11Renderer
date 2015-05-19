#pragma once

class Cb_materials_advanced_composite_Ps {
public:
	Clair::Float4x4 InverseView;
	Clair::Float4x4 InverseProj;
	int32_t SSREnabled;
	float __padding0__[3];
};
