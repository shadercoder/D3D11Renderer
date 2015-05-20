#pragma once

class Cb_materials_advanced_ssr_Ps {
public:
	Clair::Float4x4 InverseView;
	Clair::Float4x4 InverseProj;
	Clair::Float4x4 Proj;
	float Tweak;
	float __padding0__[3];
};
