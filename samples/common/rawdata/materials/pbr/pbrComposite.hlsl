#include "../packNormal.h"
#include "../numRoughnessMips.h"

Texture2D RT0 : register(t0);
Texture2D RT1 : register(t1);
Texture2D RT2 : register(t2);
Texture2D RT3 : register(t3);
TextureCube CubeMap : register(t4);
Texture2D PreviousFrame : register(t5);
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
	matrix View;
	matrix Proj;
	matrix ViewProj;
	matrix InverseViewProj;
	float3 CameraPosition;
};

float4 getGbuf(Texture2D tex, float2 uv) {
	return tex.Sample(samplerLinear, uv * float2(1.0, -1.0));
}

float3 reconstructPos(float2 uv, float d) {
	float4 p = float4(float3(uv * 2.0 - 1.0, d), 1.0);
	p = mul(InverseViewProj, p);
	return p.xyz / p.w;
}

float linearizeDepth(float d) {
	float f = 100.0;
	float n = 0.1;
	return (2 * n) / (f + n - d * (f - n));
}

float3 rayTraceReflection(float3 rayO, float3 rayD, float glossiness) {
	float mip = (1.0 - glossiness) * float(NUM_ROUGHNESS_MIPS - 1);
	float3 col = CubeMap.SampleLevel(samplerLinear, rayD, mip).rgb;
	float4 viewRayO = mul(View, float4(rayO, 1.0));
	rayO = viewRayO.xyz;
	float4 viewRayD = mul(View, float4(rayD, 0.0));
	rayD = normalize(viewRayD.xyz);
	float3 pos = rayO;
	float delta = 1.0;// + length(rayO) / 10.0;
	float2 hitPoint = float2(0.0, 0.0);
	bool isInside = false;
	bool hasHitAtLeastOnce = false;
	for (int i = 0; i < 16; ++i) {
		pos += rayD * delta;
		float4 uv = mul(Proj, float4(pos, 1.0));
		uv.xy /= uv.w;
		uv.xy = uv.xy * 0.5 + 0.5;
		if (uv.x >= 1.0 || uv.x <= 0.0 ||
			uv.y >= 1.0 || uv.y <= 0.0) {
			return col;
		}
		float3 scenePos = reconstructPos(uv.xy, getGbuf(RT0, uv.xy));
		float4 viewSceneZ = mul(View, float4(scenePos, 1.0));
		scenePos = viewSceneZ.xyz;
		// if hit
		bool newIsInside = pos.z >= scenePos.z && pos.z <= scenePos.z + 3.0;
		if ((!isInside && newIsInside) ||
			(isInside && !newIsInside)) {
			hasHitAtLeastOnce = true;
			isInside = newIsInside;
			hitPoint = uv.xy;
			delta = -0.3 * delta;
		}
	}
	if (hasHitAtLeastOnce) {
		float fade = dot(float3(0,0,-1), rayD) * 2.0;
		float maxLength = 0.7;
		fade = max(fade, 4.0 * length(hitPoint - float2(0.5, 0.5)) - maxLength);
		float3 rayCol = getGbuf(PreviousFrame, hitPoint);
		rayCol = pow(rayCol, 2.2);
		col = lerp(rayCol, col, saturate(fade));
	}
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
	float3 V = normalize(position - CameraPosition);
	float3 refl = reflect(V, normal);
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