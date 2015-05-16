#include "../packNormal.h"
#include "../numRoughnessMips.h"

Texture2D RT0 : register(t0);
Texture2D RT1 : register(t1);
Texture2D RT2 : register(t2);
Texture2D RT3 : register(t3);
TextureCube CubeMap : register(t4);
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
	matrix InverseView;
	matrix InverseProj;
};

float4 getGbuf(Texture2D tex, float2 uv) {
	return tex.Sample(samplerLinear, uv * float2(1.0, -1.0));
}

float3 reconstructPos(float2 uv, float d) {
	float4 p = float4(float3(uv * 2.0 - 1.0, d), 1.0);
	p = mul(InverseProj, p);
	return p.xyz / p.w;
}

float linearizeDepth(float d) {
	float f = 100.0;
	float n = 0.01;
	return (2 * n) / (f + n - d * (f - n));
}

float3 rayTraceReflection(float3 rayO, float3 rayD, float glossiness) {
	float mip = (1.0 - glossiness) * float(NUM_ROUGHNESS_MIPS - 1);
	float4 wsRayD = mul(InverseView, float4(rayD, 0.0));
	float3 col = CubeMap.SampleLevel(samplerLinear, wsRayD.xyz, mip).rgb;
	return col;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float3 col;
	float depth = getGbuf(RT0, psIn.Uvs).r;
	float3 position = reconstructPos(psIn.Uvs, depth);
	float3 normal = unpackNormal(getGbuf(RT1, psIn.Uvs).rg).rgb;
	float4 rt2 = getGbuf(RT2, psIn.Uvs);
	float4 rt3 = getGbuf(RT3, psIn.Uvs);
	float3 albedo = rt2.rgb;
	float emissive = rt2.a;
	float glossiness = rt3.r;
	float metalness = rt3.g;

	const float minReflectivity = 0.04;
	float reflectivity = lerp(minReflectivity, 1.0, metalness);
	float3 V = normalize(-position);
	float3 refl = reflect(-V, normal);
	float3 reflCol = rayTraceReflection(position, refl, glossiness);

	float3 ambient = CubeMap.SampleLevel(
		samplerLinear, normal, float(NUM_ROUGHNESS_MIPS - 1)).rgb;
	reflCol *= lerp(float3(1,1,1), albedo, metalness);
	col = lerp(albedo * ambient, reflCol, reflectivity);
	if (depth == 1.0) {
		col = rt2.rgb;
	}
	col = pow(col, 1.0 / 2.2);
	return float4(col, 1.0);
}