Texture2D texAlbedo : register(t0);
Texture2D texNormal : register(t1);
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
	psIn.Uvs = vsIn.Position.xy * 2.0 - 1.0;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
float3 calcLighting(float3 albedo, float3 normal) {
	//const int numLights = 16;
	//for (int i = 0; i < numLights; ++i) {
	//	float3 lpos = float3(cos(3.14 * 2.0 / float(numLights)), 1.0,
	//						 sin(3.14 * 2.0 / float(numLights)));
		float3 l = normalize(float3(-1.0, 5.0, -2.0));
		float3 col = saturate(dot(normalize(l), normal) * 1.0 / max(0.001, dot(l, l)));
	//}
	return col * albedo;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float3 col;
	float3 albedo = texAlbedo.Sample(samplerLinear, psIn.Uvs * float2(1.0, -1.0));
	col = albedo;
	if (psIn.Uvs.x > 0) {
		float3 normal = texNormal.Sample(samplerLinear, psIn.Uvs * float2(1.0, -1.0));
		if (psIn.Uvs.y > 0) {
			col = calcLighting(albedo, normal);
		} else {
			col = normal;
		}
	}
	return float4(col, 1.0);
}