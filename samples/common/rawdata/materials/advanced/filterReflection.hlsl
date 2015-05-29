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
	psIn.Uvs = vsIn.Position.xy;// * 2.0 - 1.0;
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
	psIn.Uvs = psIn.Uvs * 2 - 1;
	float dist = 1.0 / tan(FieldOfView / 2.0 * 3.14 / 180.0);
	float3 dir = CamForward * dist +
			   (CamRight * psIn.Uvs.x * Aspect) +
			   (CamUp * psIn.Uvs.y);
	dir = normalize(dir);

	const float NUM_SAMPLES = 32;
	float sampleCount = 0;
	for (float i = 0; i < NUM_SAMPLES; ++i) {
		float3 offset = hemisphereSample_uniform(hammersley2d(i, NUM_SAMPLES));
		offset = normalize(offset);
		if (dot(offset, dir) < 0) {
			offset = -offset;
		}
		float3 sampDir = lerp(dir, offset, saturate(Roughness * 0.3));
		sampDir = normalize(sampDir);
		float4 uv = mul(ViewProj, float4(sampDir, 0));
		uv.xy /= uv.w;
		uv.xy = uv.xy * .5 + .5;
		if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1 || uv.z < 0 || uv.z > 1) {
			continue;
		}
		//return float4(uv.xy, 0, 1);
		col += InputTexture.Sample(SamplerLinear, float2(uv.x, 1 - uv.y));
		++sampleCount;
	}
	col /= sampleCount;

	return col;
}