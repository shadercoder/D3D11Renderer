#include "../sampling.h"

Texture2D InputTexture : register(t0);
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
	psIn.Uvs = vsIn.Position.xy * float2(1,-1);// * 2.0 - 1.0;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b1) {
	float Roughness;
};

float4 filter(float2 uv, float3 right, float3 up, float3 forward) {
	float4 col = float4(0, 0, 0, 0);
	const int NUM_SAMPLES = 128;
	float sampleCount = Roughness * 0.00000000001;
	float4 ms = InputTexture.Sample(SamplerLinear, float2(uv.x, uv.y));
	float4 midSample = float4(pow(ms.rgb, 2.2), ms.a);
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		float2 offset = hammersley2d(i, NUM_SAMPLES) - 0.5;
		float2 u = uv + offset * 1.0 * pow(Roughness, 3);
		u.x = saturate(u.x);
		u.y = saturate(-u.y);
		float4 c = InputTexture.Sample(SamplerLinear, float2(u.x, -u.y));
		col += lerp(midSample, float4(pow(c.rgb, 2.2), c.a), 1);//saturate(1 - c.a));
		++sampleCount;
	}
	col /= sampleCount;
	col = float4(pow(col.rgb, 1 / 2.2), col.a);
	return col;
}

float4 psMain(PsIn psIn) : SV_TARGET0 {
	const float3 px = float3(1,0,0);
	const float3 nx = float3(-1,0,0);
	const float3 py = float3(0,1,0);
	const float3 ny = float3(0,-1,0);
	const float3 pz = float3(0,0,1);
	const float3 nz = float3(0,0,-1);
	//float4 col = InputTexture.Sample(SamplerLinear,
	//	psIn.Uvs * (1.0 + pow(Roughness, 2) / 10000.0)).rgba;
	//return col;
	return filter(psIn.Uvs, nz, py, px);
}