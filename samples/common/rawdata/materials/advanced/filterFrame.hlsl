#include "../sampling.h"

Texture2D ScreenInput : register(t0);
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

float3 filter(float2 uv, float3 right, float3 up, float3 forward) {
	float3 col = float3(0, 0, 0);
	col += Roughness;
	//float3 dir = normalize(forward + uv.x * right + uv.y * up);
	//const int NUM_SAMPLES = 128;
	//for (int i = 0; i < NUM_SAMPLES; ++i) {
	//	float3 offset = hemisphereSample_uniform(hammersley2d(i, NUM_SAMPLES));
	//	offset = normalize(offset);
	//	if (dot(offset, dir) < 0) {
	//		offset = -offset;
	//	}
	//	float3 sampDir = lerp(dir, offset, pow(Roughness, 2));
	//	sampDir = normalize(sampDir);
	//	//float4 sampUv = mul(Per
	//	//col += CubeMapInput.Sample(SamplerLinear, sampDir).rgb;
	//}
	//col /= float(NUM_SAMPLES);
	return col;
}

float4 psMain(PsIn psIn) : SV_TARGET0 {
	const float3 px = float3(1,0,0);
	const float3 nx = float3(-1,0,0);
	const float3 py = float3(0,1,0);
	const float3 ny = float3(0,-1,0);
	const float3 pz = float3(0,0,1);
	const float3 nz = float3(0,0,-1);
	return float4(filter(psIn.Uvs, nz, py, px), 1.0);
}