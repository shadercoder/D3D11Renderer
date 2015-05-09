#pragma once

class Cb_materials_sky_Ps {
public:
	Clair::Float3 CamRight;
	float __padding0__[1];
	Clair::Float3 CamUp;
	float __padding1__[1];
	Clair::Float3 CamForward;
	float Aspect;
	float FieldOfView;
	float __padding2__[3];
};
