#include "sampling.h"

TextureCube CubeMapInput : register(t0);
SamplerState SamplerLinear : register(s0);

struct VsIn {
	float3 Position : POSITION;
};

struct PsIn {
	float4 Position : SV_POSITION;
	float2 Uvs : UVS;
};

// -----------------------------------------------------------------------------
// VERTEX SHADER
// -----------------------------------------------------------------------------
PsIn vsMain(VsIn vsIn) {
	PsIn psIn;
	psIn.Position = float4(vsIn.Position.xy * 2.0 - 1.0, 0.0, 1.0);
	psIn.Uvs = vsIn.Position.xy * 2.0 - 1.0;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b1) {
	float Roughness;
};

struct PsOut {
	float4 Face0 : SV_TARGET0;
	float4 Face1 : SV_TARGET1;
	float4 Face2 : SV_TARGET2;
	float4 Face3 : SV_TARGET3;
	float4 Face4 : SV_TARGET4;
	float4 Face5 : SV_TARGET5;
};

float3 filter(float2 uv, float3 right, float3 up, float3 forward) {
	float3 col = float3(0, 0, 0);
	float3 dir = normalize(forward + uv.x * right + uv.y * up);
	const float NUM_SAMPLES = 512.0;
	for (float i = 0; i < NUM_SAMPLES; ++i) {
		float3 offset = hemisphereSample_uniform(hammersley2d(i, NUM_SAMPLES));
		offset = normalize(offset);
		if (dot(offset, dir) < 0) {
			offset = -offset;
		}
		float3 sampDir = lerp(dir, offset, saturate(Roughness * 0.8));
		sampDir = normalize(sampDir);
		col += CubeMapInput.Sample(SamplerLinear, sampDir).rgb;
	}
	col /= float(NUM_SAMPLES);
	return col;
}

PsOut psMain(PsIn psIn) {
	PsOut psOut;
	const float3 px = float3(1,0,0);
	const float3 nx = float3(-1,0,0);
	const float3 py = float3(0,1,0);
	const float3 ny = float3(0,-1,0);
	const float3 pz = float3(0,0,1);
	const float3 nz = float3(0,0,-1);
	psOut.Face0 = float4(filter(psIn.Uvs, nz, py, px), 1.0);
	psOut.Face1 = float4(filter(psIn.Uvs, pz, py, nx), 1.0);
	psOut.Face2 = float4(filter(psIn.Uvs, px, nz, py), 1.0);
	psOut.Face3 = float4(filter(psIn.Uvs, px, pz, ny), 1.0);
	psOut.Face4 = float4(filter(psIn.Uvs, px, py, pz), 1.0);
	psOut.Face5 = float4(filter(psIn.Uvs, nx, py, nz), 1.0);
	return psOut;
}