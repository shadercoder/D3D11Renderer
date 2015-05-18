#include "../packNormal.h"

Texture2D RT0 : register(t0);
Texture2D RT1 : register(t1);
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
	matrix Proj;
	bool SSREnabled;
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

float rand(float2 uv){
	return frac(sin(dot(uv.xy, float2(12.9898,78.233))) * 43758.5453);
}

float2 rayTraceGetUv(float3 v) {
	float4 uv = mul(Proj, float4(v, 1.0));
	uv.xy /= uv.w;
	return uv.xy * 0.5 + 0.5;
}

float3 getReflectionColor(float3 rayO, float3 rayD, float glossiness) {
	float mip = (1.0 - glossiness) * float(NUM_ROUGHNESS_MIPS - 1);
	float4 wsRayD = mul(InverseView, float4(rayD, 0.0));
	float3 col = CubeMap.SampleLevel(samplerLinear, wsRayD.xyz, mip).rgb;
	float delta = 0.5 + 0.1 * rand(rayO.xy);// * lerp(0.5, 1.0, rand(rayO.xy)) * length(rayO);
	float3 pos = rayO;
	float2 hitUv = float2(0,0);
	float3 prevPos;
	float lastSceneZ;
	float lastPosZ;
	bool hit = false;
	float t = 0.0;
	float prevT = 0.0;
	for (float i = 0; i < 16; ++i) {
		prevT = t;
		t += delta;
		pos = rayO + rayD * t;
		float2 uv = rayTraceGetUv(pos);
		if (uv.x >= 1.0 || uv.x <= 0.0 ||
			uv.y >= 1.0 || uv.y <= 0.0) {
			break;
		}
		float3 scenePos = reconstructPos(uv, getGbuf(RT0, uv).r);
		if (pos.z >= scenePos.z && pos.z <= scenePos.z + 2.0) {
			lastSceneZ = scenePos.z;
			lastPosZ = pos.z;
			hit = true;
			hitUv = uv;
			t = prevT;
			delta *= 0.5;
		}
	}
	if (hit) {// && lastPosZ < lastSceneZ + VoxelDepth) {
		//mip = max(0, mip - float(NUM_ROUGHNESS_MIPS) * (1.0 - t / 5.0));
		float3 rayCol = PreviousFrame.SampleLevel(
			samplerLinear, hitUv * float2(1,-1), mip).rgb;
		rayCol = pow(rayCol, 2.2);
		float fade = 0.0;//t * 0.1;// * (1.0 - glossiness);//length(float2(0.5,0.5) - hitUv) * 2.0 * (4.0 - glossiness * 4.0);
		col = lerp(rayCol, col, saturate(fade));
	}
	//float l = length(float2(0.5,0.5) - rayTraceGetUv(rayO));
	//col = lerp(col, reflCol, saturate(l));
	//col = float3(rayTraceGetUv(rayO), 0.0);//col.z);
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
	float3 reflCol = getReflectionColor(position, refl, glossiness);

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