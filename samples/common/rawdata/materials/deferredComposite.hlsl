#include "numLights.h"

Texture2D texAlbedo : register(t0);
Texture2D texNormal : register(t1);
Texture2D texPosition : register(t2);
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
	float4 LightDiffuseColors[NUM_LIGHTS];
	float4 LightPositions[NUM_LIGHTS];
	float3 CameraPosition;
	bool DrawGBuffers;
};

float3 calcLighting(float3 albedo, float3 normal, float3 position) {
	const float3 amb = (1.0, 1.0, 1.0) * 0.00;
	float3 view = normalize(CameraPosition - position);
	float3 col = float3(0, 0, 0);
	for (int i = 0; i < NUM_LIGHTS; ++i) {
		float3 light = LightPositions[i].xyz - position;
		float lightDist2 = max(0.001, dot(light, light));
		light = normalize(light);

		float diff = saturate(dot(light, normal));
		diff *= LightDiffuseColors[i].a / lightDist2;

		float3 H = normalize(light + view);
		float NdotH = max(dot(normal, H), 0.0);
		float spec = pow(saturate(NdotH), 500.0);

		col += lerp(
			diff * LightDiffuseColors[i].rgb,
			spec * LightDiffuseColors[i].rgb,
			0.50);
	}
	return pow(col * albedo + amb * albedo, 1.0 / 2.2);
}

float3 getGbuf(Texture2D tex, float2 uv) {
	return tex.Sample(samplerLinear, uv * float2(1.0, -1.0)).rgb;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float3 col;
	float3 albedo = getGbuf(texAlbedo, psIn.Uvs);
	float3 normal = getGbuf(texNormal, psIn.Uvs);
	float3 position = getGbuf(texPosition, psIn.Uvs);
	col = calcLighting(albedo, normal, position);
	if (DrawGBuffers) {
		const float numGbuf = 3.0;
		const float ratio = 1.0 / numGbuf;
		if (psIn.Uvs.y < ratio) {
			if (psIn.Uvs.x < ratio) {
				float2 uv = psIn.Uvs / ratio;
				col = getGbuf(texPosition, uv);
			} else if (psIn.Uvs.x < 2.0 * ratio) {
				float2 uv = (psIn.Uvs - float2(ratio, 0.0)) / ratio;
				col = getGbuf(texAlbedo, uv);
			} else {
				float2 uv = (psIn.Uvs - float2(ratio * 2.0, 0.0)) / ratio;
				col = getGbuf(texNormal, uv);
			}
		}
	}
	return float4(col, 1.0);
}