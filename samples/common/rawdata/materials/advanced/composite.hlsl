#include "../packNormal.h"
#include "../numRoughnessMips.h"
#include "numLights.h"

Texture2D RT0 : register(t0);
Texture2D RT1 : register(t1);
Texture2D RT2 : register(t2);
Texture2D RT3 : register(t3);
Texture2D ReflectionBuffer : register(t4);
TextureCube CubeMap : register(t5);
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
	psIn.Uvs = vsIn.Position.xy * float2(1,-1);// * 2.0 - 1.0;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------

cbuffer Buf : register(b1) {
	matrix InverseView;
	matrix InverseProj;
	matrix View;
	float4 LightColors[NUM_LIGHTS];
	float4 LightPositions[NUM_LIGHTS];
	float Ambient;
	bool SSREnabled;
	bool LightsEnabled;
};

float4 getGbuf(Texture2D tex, float2 uv) {
	return tex.Sample(samplerLinear, uv);
}

float3 reconstructPos(float2 uv, float d) {
	float4 p = float4(float3(uv * float2(1,-1) * 2.0 - 1.0, d), 1.0);
	p = mul(InverseProj, p);
	return p.xyz / p.w;
}

float3 getReflectionColor(float2 uv, float3 rayD, float glossiness) {
	float mip = (1.0 - glossiness) * float(NUM_ROUGHNESS_MIPS - 1);
	float4 wsRayD = mul(InverseView, float4(rayD, 0.0));
	float3 col = CubeMap.SampleLevel(samplerLinear, wsRayD.xyz, mip).rgb;
	if (!SSREnabled) {
		return col;
	}
	//uv += rayD.xz * 0.01;
	float hit = ReflectionBuffer.SampleLevel(samplerLinear, uv, 0).a;
	float4 reflValue = ReflectionBuffer.SampleLevel(samplerLinear, uv, mip);
	reflValue = float4(pow(reflValue.rgb, 2.2), reflValue.a);
	
	float fade = 0.0;//3.0 * length(float2(.5,-.5) - uv);
	fade = max(1.0 - reflValue.a, fade);
	col = lerp(reflValue.rgb, col, saturate(fade));
	return col;
}

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
		float intensity = LightColors[i].a;
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
			diff * LightColors[i].rgb,
			spec * LightColors[i].rgb,
			0.50);
	}
	col = col * albedo + amb * albedo;
	col += albedo * emissive * 32.0;
	return pow(col, 1.0 / 2.2);
}

float3 calcAmbient(float3 N) {
	return CubeMap.SampleLevel(samplerLinear,
							   N,
							   float(NUM_ROUGHNESS_MIPS - 1)).rgb;
}

float calcPhong(float3 N, float3 L, float3 R, float gloss) {
	float phong = 0;
	if (dot(N, L) > 0) {
		// Using that pow(max_phong_power, roughness) is from
		// http://renderwonk.com/publications/s2010-shading-course/hoffman/s2010_physically_based_shading_hoffman_b.pdf
		float a = 0.001 + pow(2048, pow(gloss, 2));
		phong = pow(saturate(dot(R, L)), a);
		phong *= (a + 2) / (2 * 3.1415);
		phong /= 20.0;
	}
	return phong;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float3 col;
	float depth = getGbuf(RT0, psIn.Uvs).r;
	float3 position = reconstructPos(psIn.Uvs, depth);
	float3 N = unpackNormal(getGbuf(RT1, psIn.Uvs).rg).rgb;
	float4 rt2 = getGbuf(RT2, psIn.Uvs);
	float4 rt3 = getGbuf(RT3, psIn.Uvs);
	float3 albedo = rt2.rgb;
	float emissive = rt2.a;
	float glossiness = rt3.r;
	float metalness = rt3.g;
	const float minReflectivity = 0.04;
	float reflectivity = lerp(minReflectivity, 1.0, metalness);
	float3 V = normalize(-position);
	float3 R = reflect(-V, N);
	float3 matReflCol = lerp(float3(1,1,1), albedo, metalness);

	float3 reflCol = getReflectionColor(psIn.Uvs, R, glossiness);

	float3 ambient = calcAmbient(N);
	//reflCol *= matReflCol;
	//col = lerp(albedo * ambient, reflCol, reflectivity);

	float3 diffuseColor = 0;
	float3 specularColor = 0;
	if (LightsEnabled) {
		for (int i = 0; i < NUM_LIGHTS; ++i) {
			float4 viewLightPos = mul(View, float4(LightPositions[i].xyz, 1.0));
			float3 L = viewLightPos.xyz - position;
			float lightDist2 = max(0.001, dot(L, L));
			L = normalize(L);
			float3 lightCol = LightColors[i].rgb * LightColors[i].a;

			float diff = saturate(dot(L, N)) / lightDist2;
			float phong = calcPhong(N, L, R, glossiness);
			diffuseColor += diff * lightCol;
			specularColor += phong * lightCol;
		}
	}
	diffuseColor += ambient * Ambient;
	specularColor += reflCol;
	col = lerp(diffuseColor, specularColor, reflectivity) + emissive * 3.0;
	col *= albedo;
	if (depth == 1.0) {
		col = rt2.rgb;
	}
	col = pow(col, 1.0 / 2.2);
	//col *= 0.000001;
	//col += calcLighting(albedo, normal, position, 0);
	//float3 x = max(0, col - .004);
	//col = (x * (6.2 * x + .5)) / (x * (6.2 * x + 1.7) + .06);
	return float4(col, 1.0);
}