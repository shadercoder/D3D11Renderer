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

float4 getReflectionColor(float3 rayO, float3 rayD, float2 gUv) {
	float4 col = float4(0,0,0,0);
	float4 startPos4D = mul(Proj, float4(rayO, 1));
	float4 endPos4D = mul(Proj, float4(rayO + rayD, 1));

	float4 pos4D = startPos4D;
	float3 pos = pos4D.xyz / pos4D.w;
	float delta = .4 + 0.1 * rand(rayO.xy);
	float2 hitUv = float2(0,0);
	bool hit = false;
	float t = 0;
	for (float i = 0; i < 20; ++i) {
		t += delta;
		pos4D = startPos4D + (endPos4D - startPos4D) * t;
		pos = pos4D.xyz / pos4D.w;
		pos.xy = pos.xy * .5 + .5;
		pos.z = linearizeDepth(pos.z);
		if (pos.x < 0 || pos.x > 1 || pos.y < 0 || pos.y > 1.0) break;
		float sceneZ = linearizeDepth(textureSample(RT0, pos.xy).r);
		if (pos.z > sceneZ && pos.z <= sceneZ + 0.01 + delta / 40) {
			hit = true;
			hitUv = pos.xy;
			t -= delta;
			delta *= .5;
			//break;
		}
	}
	if (hit) {
		float3 rayCol = PreviousFrame.SampleLevel(
			samplerLinear, float2(hitUv.x, 1 - hitUv.y), 0).rgb;
		float fade = -1.5 + 5.0 * length(float2(.5, .5) - pos.xy);
		//fade = max(fade, lerp(0, 1.0, t / (.4 * 32)));
		col = float4(rayCol, saturate(1.0 - fade));
	}
	return col;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float2 uv = psIn.Uvs;
	//psIn.Uvs = float2(.5,.5);
	float depth = textureSample(RT0, psIn.Uvs).r;
	float3 position = reconstructPos(psIn.Uvs, depth);
	float3 normal = unpackNormal(textureSample(RT1, psIn.Uvs).rg).rgb;

	float3 V = normalize(-position);
	float3 refl = reflect(-V, normal);
	float4 reflCol = getReflectionColor(position, refl, uv);
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