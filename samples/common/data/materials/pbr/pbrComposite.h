#pragma once

class Cb_materials_pbr_pbrComposite_Ps {
public:
	Clair::Float4x4 ViewProj;
	Clair::Float4x4 InverseViewProj;
	Clair::Float3 CameraPosition;
	float __padding0__[1];
};
