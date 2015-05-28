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
	psIn.Uvs = vsIn.Position.xy * 2.0 - 1.0;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b1) {
	matrix ViewProj;
	float3 CamRight;
	float3 CamUp;
	float3 CamForward;
	float Roughness;
	float Aspect;
	float FieldOfView;
};

float4 psMain(PsIn psIn) : SV_TARGET0 {
	float4 col = float4(0, 0, 0, 0);
	const int NUM_SAMPLES = 32;
	float sampleCount = Roughness * 0.00000000001;
	float4 ms = InputTexture.Sample(SamplerLinear, float2(psIn.Uvs.x, psIn.Uvs.y));
	float4 midSample = float4(pow(ms.rgb, 2.2), ms.a);
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		float2 offset = hammersley2d(i, NUM_SAMPLES) - 0.5;
		float2 u = psIn.Uvs + offset * 1.0 * pow(Roughness, 2);
		u.x = saturate(u.x);
		u.y = saturate(-u.y);
		float4 c = InputTexture.Sample(SamplerLinear, float2(u.x, -u.y));
		col += lerp(midSample, float4(pow(c.rgb, 2.2), c.a), 1);//saturate(1 - c.a));
		++sampleCount;
	}
	col /= sampleCount;
	col = float4(pow(col.rgb, 1 / 2.2), col.a);

	float dist = 1.0 / tan(FieldOfView / 2.0 * 3.14 / 180.0);
	float3 dir = CamForward * dist +
			   (CamRight * psIn.Uvs.x * Aspect) +
			   (CamUp * psIn.Uvs.y);
	dir = normalize(dir);
	//float3 dir = normalize(CamForward + psIn.Uvs.x * CamRight + psIn.Uvs.y * CamUp);
	float4 finalUv = mul(ViewProj, float4(dir, 0));
	finalUv /= finalUv.w;
	finalUv = finalUv * .5 + .5;
	col.rgb = InputTexture.Sample(SamplerLinear, float2(finalUv.x, 1 - finalUv.y));
	//col *= .0000001;
	//col.rg += finalUv.xy;
	//col.a = 1.0;
	return col;
}