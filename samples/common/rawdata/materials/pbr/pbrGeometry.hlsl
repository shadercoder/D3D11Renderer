#include "../packNormal.h"

struct VsIn {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

struct PsIn {
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
};

// -----------------------------------------------------------------------------
// VERTEX SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b0) {
	matrix World;
	matrix View;
	matrix Projection;
};

PsIn vsMain(VsIn vsIn) {
	PsIn psIn;
	psIn.Position = mul(Projection, mul(View, mul(World, float4(vsIn.Position, 1.0))));
	psIn.Normal = mul(World, float4(vsIn.Normal, 0.0)).xyz;;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Material : register(b1) {
	float3 Albedo;
	float Emissive;
	float Glossiness;
	float Metalness;
}

struct PsOut {
	float2 RT1 : SV_TARGET0;
	float4 RT2 : SV_TARGET1;
	float4 RT3 : SV_TARGET2;
};

PsOut psMain(PsIn psIn) {
	PsOut psOut;
	psOut.RT1 = packNormal(normalize(psIn.Normal));
	psOut.RT2 = float4(Albedo, Emissive);
	psOut.RT3 = float4(Glossiness, Metalness, 0.0, 0.0);
	return psOut;
}