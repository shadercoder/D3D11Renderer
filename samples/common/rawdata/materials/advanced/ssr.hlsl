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
	psIn.Uvs = vsIn.Position.xy;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b1) {
	matrix InverseView;
	matrix InverseProj;
	matrix Proj;
	float Tweak;
};

float4 textureSample(Texture2D tex, float2 uv) {
	return tex.Sample(samplerLinear, float2(uv.x, 1 - uv.y));
}

float3 reconstructPos(float2 uv, float d) {
	float4 p = float4(float3(uv * 2.0 - 1.0, d), 1.0);
	p = mul(InverseProj, p);
	return p.xyz / p.w;
}

float rand(float2 uv){
	return frac(sin(dot(uv.xy, float2(12.9898,78.233))) * 43758.5453);
}

float2 rayTraceGetUv(float3 v) {
	float4 uv = mul(Proj, float4(v, 1.0));
	uv.xy /= uv.w;
	uv.xy = uv.xy * 0.5 + 0.5;
	return uv;
}

float linearizeDepth(float d) {
	float f = 100.0;
	float n = 0.01;
	return (2 * n) / (f + n - d * (f - n));
}

float4 getReflectionColor(float3 rayO, float3 rayD) {
	float3 col = float3(0,0,0);
	float2 startUv = rayTraceGetUv(rayO);
	float2 endUv = rayTraceGetUv(rayO + rayD);
	float startDepth = linearizeDepth(textureSample(RT0, startUv).r);
	float4 endPos4D = mul(Proj, float4(rayO + rayD, 1));
	endPos4D.xyz /= endPos4D.w;
	float endDepth = linearizeDepth(endPos4D.z);
	float3 start = float3(startUv, startDepth);
	float3 end = float3(endUv, endDepth);
	//return float4(float3(1,1,1) * endDepth, 1);
	//return float4(end - start, 1);
	
	//return float4(float3(1,1,1) * startDepth, 1);
	//return float4(end.xy,0, 1);
	float delta = 1 / length(endUv - startUv);
	float3 pos = start;
	float2 hitUv = float2(0,0);
	bool hit = false;
	float t = 0.0;
	for (float i = 0; i < 32; ++i) {
		t += delta * 0.01;
		pos = start + (end - start) * t;
		//pos = start;
		if (pos.x < 0 || pos.x > 1 || pos.y < 0 || start.y > 1.0) break;
		float sceneZ = linearizeDepth(textureSample(RT0, pos.xy).r);
		//if (rayO.x < 0) {
		//	//return float4(float3(1,1,1) * sceneZ, 1);
		//	//return float4(pos.xy,0, 1);
		//	return float4(float3(1,1,1) * start.z, 1);
		//} else {
		//	return float4(float3(1,1,1) * pos.z, 1);
		//}
		//return float4(PreviousFrame.Sample(samplerLinear, float2(pos.x, 1 - pos.y)).rgb, 1);
		if (pos.z > sceneZ && pos.z <= sceneZ + 0.01) {
			hit = true;
			hitUv = pos.xy;
			break;
		}
	}
	float a = 0;
	if (hit) {
		float3 rayCol = PreviousFrame.SampleLevel(
			samplerLinear, float2(hitUv.x, 1 - hitUv.y), 0).rgb;
		col = rayCol;
		a = 1;
	}
	return float4(col, a);
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float depth = textureSample(RT0, psIn.Uvs).r;
	float3 position = reconstructPos(psIn.Uvs, depth);
	float3 normal = unpackNormal(textureSample(RT1, psIn.Uvs).rg).rgb;

	float3 V = normalize(-position);
	float3 refl = reflect(-V, normal);
	float4 reflCol = getReflectionColor(position, refl);
	float4 col = pow(reflCol, 1.0 / 2.2);
	//float2 testUv = rayTraceGetUv(reconstructPos(psIn.Uvs, depth));
	//if (psIn.Uvs.y > 0.0)
	//	return float4(testUv,0,1);
		//return float4(float3(1,1,1) * linearizeDepth(depth), 1);
		//return float4(position,1);
		//return float4(reconstructPos(rayTraceGetUv(position), depth),1);
	//if (psIn.Uvs.x < 1.0)
	//return float4(psIn.Uvs, 0, 1);
	//return float4(position, 1);
	//reflCol *= 0.00001;
	//reflCol += float4(1,0,0,1);
	return reflCol;
}