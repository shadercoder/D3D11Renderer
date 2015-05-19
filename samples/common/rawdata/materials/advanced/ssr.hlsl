#include "../packNormal.h"

Texture2D RT0 : register(t0);
Texture2D RT1 : register(t1);
Texture2D PreviousFrame : register(t2);
Texture2D CubeMap : register(t3);
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
	matrix Proj;
};

float3 reconstructPos(float2 uv, float d) {
	float4 p = float4(float3(uv * float2(1,-1) * 2.0 - 1.0, d), 1.0);
	p = mul(InverseProj, p);
	return p.xyz / p.w;
}

float rand(float2 uv){
	return frac(sin(dot(uv.xy, float2(12.9898,78.233))) * 43758.5453);
}

float2 rayTraceGetUv(float3 v) {
	float4 uv = mul(Proj, float4(v, 1.0));
	uv.xy /= uv.w;
	return uv.xy *float2(1,-1)* 0.5 + 0.5;
}

float4 getReflectionColor(float3 rayO, float3 rayD) {
	float3 col = float4(0,0,0,0);
	float delta = 0.4 + 0.4 * rand(rayO.xy);// * lerp(0.5, 1.0, rand(rayO.xy)) * length(rayO);
	float3 pos = rayO;
	float2 hitUv = float2(0,0);
	float3 prevPos;
	float lastSceneZ;
	float lastPosZ;
	bool hit = false;
	float t = 0.0;
	float prevT = 0.0;
	for (float i = 0; i < 32; ++i) {
		prevT = t;
		t += delta;
		pos = rayO + rayD * (t + delta * 0.5);
		float2 uv = rayTraceGetUv(pos);
		if (uv.x >= 1.0 || uv.x <= 0.0 ||
			uv.y >= 1.0 || uv.y <= 0.0) {
			break;
		}
		float depth = RT0.Sample(samplerLinear, uv).r;
		float3 scenePos = reconstructPos(uv, depth);
		if (pos.z >= scenePos.z) {//&& pos.z <= scenePos.z + 2.0) {
			lastSceneZ = scenePos.z;
			lastPosZ = pos.z;
			hit = true;
			hitUv = uv;
			t = prevT;
			delta *= 0.5;
		}
	}
	float a = 0;
	if (hit && lastPosZ < lastSceneZ + 0.1) {
		//mip = max(0, mip - float(NUM_ROUGHNESS_MIPS) * (1.0 - t / 5.0));
		float3 rayCol = PreviousFrame.SampleLevel(
			samplerLinear, hitUv, 0).rgb;
		col = rayCol;
		//rayCol = pow(rayCol, 2.2);
		//float fade = 0.0;//t * 0.1;// * (1.0 - glossiness);//length(float2(0.5,0.5) - hitUv) * 2.0 * (4.0 - glossiness * 4.0);
		//col = lerp(rayCol, col, saturate(fade));
		a = 1;
	}
	//float l = length(float2(0.5,0.5) - rayTraceGetUv(rayO));
	//col = lerp(col, reflCol, saturate(l));
	//col = float3(rayTraceGetUv(rayO), 0.0);//col.z);
	return float4(col, a);
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float depth = RT0.Sample(samplerLinear, psIn.Uvs).r;
	float3 position = reconstructPos(psIn.Uvs, depth);
	float3 normal = unpackNormal(RT1.Sample(samplerLinear, psIn.Uvs).rg).rgb;

	float3 V = normalize(-position);
	float3 refl = reflect(-V, normal);
	float4 reflCol = getReflectionColor(position, refl);
	float4 col = pow(reflCol, 1.0 / 2.2);
	//reflCol *= 0.00001;
	//reflCol += float4(1,0,0,1);
	return reflCol;
}