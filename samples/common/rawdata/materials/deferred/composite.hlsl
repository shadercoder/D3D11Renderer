#include "numLights.h"
#include "../packNormal.h"

Texture2D RT0 : register(t0);
Texture2D RT1 : register(t1);
Texture2D RT2 : register(t2);
SamplerState samplerLinear : register(s0);

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
	matrix InverseProj;
	matrix View;
	float4 LightDiffuseColors[NUM_LIGHTS];
	float4 LightPositions[NUM_LIGHTS];
	bool DrawGBuffers;
};

float3 calcLighting(float3 albedo, float3 normal, float3 position, float emissive) {
	const float3 amb = float3(1.0, 1.0, 1.0) * 0.002;
	float3 view = normalize(-position);
	float3 col = float3(0, 0, 0);
	for (int i = 0; i < NUM_LIGHTS; ++i) {
		float4 viewLightPos = mul(View, float4(LightPositions[i].xyz, 1.0));
		float3 light = viewLightPos.xyz - position;
		float lightDist2 = max(0.001, dot(light, light));
		light = normalize(light);

		float diff = saturate(dot(light, normal));
		float intensity = LightDiffuseColors[i].a;
		diff *= intensity / lightDist2;

		float3 H = normalize(light + view);
		float NdotH = dot(normal, H);
		float spec = 0;
		if (dot(light, normal) > 0) {
			const float a = 200;
			spec = pow(saturate(NdotH), a);
			spec *= intensity;
			// Normalization factor for energy conservation
			spec *= (a + 8) / (8 * 3.1415);
		}

		col += lerp(
			diff * LightDiffuseColors[i].rgb,
			spec * LightDiffuseColors[i].rgb,
			0.50);
	}
	col = col * albedo + amb * albedo;
	col += albedo * emissive * 32.0;
	return pow(col, 1.0 / 2.2);
}

float4 getGbuf(Texture2D tex, float2 uv) {
	return tex.Sample(samplerLinear, uv * float2(1.0, -1.0));
}

float linearizeDepth(float d) {
	float f = 100.0;
	float n = 0.01;
	return (2 * n) / (f + n - d * (f - n));
}

float3 reconstructPos(float2 uv, float d) {
	float4 p = float4(float3(uv * 2.0 - 1.0, d), 1.0);
	p = mul(InverseProj, p);
	return p.xyz / p.w;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float3 col;
	float3 position = reconstructPos(psIn.Uvs, getGbuf(RT0, psIn.Uvs).r);
	float3 normal = unpackNormal(getGbuf(RT1, psIn.Uvs).rg).rgb;
	float4 albedo = getGbuf(RT2, psIn.Uvs);
	col = calcLighting(albedo.rgb, normal, position, albedo.a);
	if (DrawGBuffers) {
		const float numGbuf = 4.0;
		const float ratio = 1.0 / numGbuf;
		if (psIn.Uvs.y < ratio) {
			if (psIn.Uvs.x < ratio) {
				float2 uv = psIn.Uvs / ratio;
				col = linearizeDepth(getGbuf(RT0, uv).r);
			} else if (psIn.Uvs.x < 2.0 * ratio) {
				float2 uv = (psIn.Uvs - float2(ratio, 0.0)) / ratio;
				col = reconstructPos(uv, getGbuf(RT0, uv).r) / 2.0;
			} else if (psIn.Uvs.x < 3.0 * ratio) {
				float2 uv = (psIn.Uvs - float2(ratio * 2.0, 0.0)) / ratio;
				col = getGbuf(RT2, uv).rgb;
			} else {
				float2 uv = (psIn.Uvs - float2(ratio * 3.0, 0.0)) / ratio;
				float3 n = unpackNormal(getGbuf(RT1, uv).rg).rgb;
				col = n * 0.5 + 0.5;
			}
		}
	}
	return float4(col, 1.0);
}