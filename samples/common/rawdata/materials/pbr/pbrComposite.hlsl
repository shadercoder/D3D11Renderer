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
	float VoxelDepth;
	float2 ScreenDimensions;
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
	float n = 0.01;
	return (2 * n) / (f + n - d * (f - n));
}

float rand(float2 uv){
	return frac(sin(dot(uv.xy, float2(12.9898,78.233))) * 43758.5453);
}

float2 rayTraceGetUv(float3 v) {
	float4 uv = mul(Proj, float4(v, 1.0));
	uv.xy /= uv.w;
	return uv.xy;// * 0.5 + 0.5;
}

float3 rayTraceReflection(float3 rayO, float3 rayD, float glossiness) {
	float mip = (1.0 - glossiness) * float(NUM_ROUGHNESS_MIPS - 1);
	float3 col = CubeMap.SampleLevel(samplerLinear, rayD, mip).rgb;
	float4 viewRayO = mul(View, float4(rayO, 1.0));
	rayO = viewRayO.xyz;
	float4 viewRayD = mul(View, float4(rayD, 0.0));
	rayD = normalize(viewRayD.xyz);// * 0.1;
	// calc delta based on screen space direction
	//float2 ssStart = rayTraceGetUv(rayO);
	//float2 ssEnd = rayTraceGetUv(rayO + rayD);
	//float ssDist = length(ssEnd - ssStart);// * ScreenDimensions);
	//float delta = ssDist;
	//if (ssStart.x >= 1.0 || ssStart.x <= -1.0 ||
	//	ssStart.y >= 1.0 || ssStart.y <= -1.0 ||
	//	ssEnd.x >= 1.0 || ssEnd.x <= -1.0 ||
	//	ssEnd.y >= 1.0 || ssEnd.y <= -1.0) {
	//	delta = 0.01;
	//}
	float delta = 0.5 + 0.5 * rand(rayO.xy);// * lerp(0.5, 1.0, rand(rayO.xy)) * length(rayO);
	float3 pos = rayO;
	float2 hitUv;
	float3 prevPos;
	float lastSceneZ;
	float lastPosZ;
	bool hit = false;
	for (float i = 0; i < 32.0; ++i) {
		prevPos = pos;
		pos += rayD * delta;
		float2 uv = rayTraceGetUv(pos) * 0.5 + 0.5;
		if (uv.x >= 1.0 || uv.x <= 0.0 ||
			uv.y >= 1.0 || uv.y <= 0.0) {
			return col;
		}
		float3 scenePos = reconstructPos(uv, getGbuf(RT0, uv));
		float4 viewSceneZ = mul(View, float4(scenePos, 1.0));
		scenePos = viewSceneZ.xyz;
		if (pos.z >= scenePos.z && pos.z <= scenePos.z + VoxelDepth) {
			lastSceneZ = scenePos.z;
			lastPosZ = pos.z;
			hit = true;
			hitUv = uv;
			pos = prevPos;
			delta *= 0.5;
			break;
		}
	}
	if (hit) {// && lastPosZ < lastSceneZ + VoxelDepth) {
		float3 rayCol = PreviousFrame.SampleLevel(
			samplerLinear, hitUv * float2(1,-1), 0).rgb;//mip).rgb;
		rayCol = pow(rayCol, 2.2);
		col = rayCol;
		//float d = lastPosZ - lastSceneZ;
		//if (d < VoxelDepth * 10.0)
		//return float3(1,1,1) * (lastPosZ - lastSceneZ) / 10.0;
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
	//col *= 0.00001;
	//col += reflCol;
	return float4(col, 1.0);
}