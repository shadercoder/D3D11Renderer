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
	return uv.xy *float2(1,1)* 0.5 + 0.5;
}

float4 getReflectionColor(float3 rayO, float3 rayD) {
	float3 col = float4(0,0,0,0);
	float delta = 0.4;// + 0.4 * rand(rayO.xy);
	float3 pos = rayO;
	float2 hitUv = float2(0,0);
	bool hit = false;
	float t = 0.0;
	for (float i = 0; i < 32; ++i) {
		pos = rayO + rayD * t;
		t += delta;
		float2 uv = rayTraceGetUv(pos);
		if (uv.x > 1.0 || uv.x < 0.0 ||
			uv.y > 1.0 || uv.y < 0.0) {
			break;
		}
		float depth = RT0.Sample(samplerLinear, uv).r;
		return float4(uv, 0, 1);
		float3 scenePos = reconstructPos(uv, depth);
		//return float4(scenePos, 1);
		if (pos.z >= scenePos.z) {//&& pos.z <= scenePos.z + 2.0) {
			hit = true;
			hitUv = uv;
			break;
		}
	}
	float a = 0;
	if (hit){// && lastPosZ < lastSceneZ + 0.1) {
		float3 rayCol = PreviousFrame.SampleLevel(
			samplerLinear, hitUv, 0).rgb;
		col = rayCol;
		a = 1;
	}
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