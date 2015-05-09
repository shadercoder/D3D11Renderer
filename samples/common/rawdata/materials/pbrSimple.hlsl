#include "numRoughnessMips.h"

TextureCube texAlbedo : register(t0);
SamplerState samplerLinear : register(s0);

struct VsIn {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

struct PsIn {
	float4 Position : SV_POSITION;
	float3 WPosition : WPOSITION;
	float3 WNormal : WNORMAL;
};

// -----------------------------------------------------------------------------
// VERTEX SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b0) {
	matrix World;
	matrix View;
	matrix Projection;
	float3 CameraPos;
}

PsIn vsMain(VsIn vsIn) {
	PsIn psIn;
	psIn.Position =
			mul(Projection, mul(View, mul(World, float4(vsIn.Position, 1.0))));
	psIn.WPosition = mul(World, float4(vsIn.Position, 1.0)).xyz;
	psIn.WNormal = mul(World, float4(vsIn.Normal, 0.0)).xyz;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Material : register(b1) {
	float Reflectivity;
	float Roughness;
	float Metalness;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float3 n = normalize(psIn.WNormal);
	float3 l = normalize(float3(-1.0, 5.0, -2.0));
	float3 V = normalize(psIn.WPosition - CameraPos);

	float3 albedo = float3(0.0, 1.0, 0.0);
	float diff = dot(l, n) * 1.0 / max(0.001, dot(l, l));
	diff = saturate(diff) + float3(0.2, 0.4, 0.6) * 0.05;
	
	float3 refl = reflect(V, n);
	float actualMip = texAlbedo.Sample(samplerLinear, refl).a;
	float roughnessMip = Roughness * float(NUM_ROUGHNESS_MIPS - 1);
	// TODO: probably should use max(actualMip, roughnessMip) but seems too blurry
	float3 reflCol =
		texAlbedo.SampleLevel(samplerLinear, refl, roughnessMip).rgb;
	reflCol = pow(reflCol, 2.2);
	reflCol = lerp(reflCol, reflCol * albedo, Metalness);
	float3 H = normalize(refl + V);
	float HdotV = dot(H, V);
	float finalReflectivity = Reflectivity;// + (1 - Reflectivity) * pow(HdotV, 5);
	
	float3 col = albedo * diff;
	col = lerp(col, reflCol, finalReflectivity);
	col += 0.01;
	col = pow(col, 1.0 / 2.2);
	return float4(col, 1.0);
}