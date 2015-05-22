#include "../packNormal.h"
Texture2D Albedo_Metal : register(t0);
Texture2D Normal_Gloss : register(t1);
SamplerState SamplerLinear : register(s0);

struct VsIn {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Uvs : TEXCOORDS;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
};

struct PsIn {
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Uvs : UVS;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
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
	psIn.Normal = mul(View, mul(World, float4(vsIn.Normal, 0.0))).xyz;
	psIn.Uvs = vsIn.Uvs;
	psIn.Tangent = mul(View, mul(World, float4(vsIn.Tangent, 0.0))).xyz;
	psIn.Bitangent = mul(View, mul(World, float4(vsIn.Bitangent, 0.0))).xyz;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Material : register(b1) {
	float Emissive;
}

float4 sampleTexture(Texture2D tex, float2 uv) {
	return tex.Sample(SamplerLinear, float2(uv.x, 1 - uv.y));
}

struct PsOut {
	float2 RT1 : SV_TARGET0;
	float4 RT2 : SV_TARGET1;
	float4 RT3 : SV_TARGET2;
};

PsOut psMain(PsIn psIn) {
	float4 albedo_metal = sampleTexture(Albedo_Metal, psIn.Uvs);
	float4 normal_gloss = sampleTexture(Normal_Gloss, psIn.Uvs);

	float3 albedo = pow(albedo_metal.rgb, 2.2);
	float metal = pow(albedo_metal.a, 2.2);
	float gloss = pow(normal_gloss.a, 2);
	float3 texNormal = normal_gloss.rgb * 2 - 1;
	texNormal.y *= -1;
	float3 t = psIn.Tangent;
	float3 b = psIn.Bitangent;
	float3 n = psIn.Normal;
	float3x3 tangentToView = float3x3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z);
	float3 normal = normalize(mul(tangentToView, texNormal));
	PsOut psOut;
	psOut.RT1 = packNormal(normal);
	psOut.RT2 = float4(albedo, Emissive);
	psOut.RT3 = float4(gloss, metal, 0.0, 0.0);
	return psOut;
}