Texture2D InputTexture : register(t0);
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
float4 psMain(PsIn psIn) : SV_TARGET {
	float3 col = InputTexture.SampleLevel(
		samplerLinear, psIn.Uvs * float2(1.0, -1.0), 0);
	
	//float3 bloom = float3(0,0,0);
	//const float numSamples = 4;
	//float sampleCount = 0;
	//for (float x = -numSamples; x < numSamples; ++x) {
	//	for (float y = -numSamples; y < numSamples; ++y) {
	//		float2 uv = psIn.Uvs + float2(x, y) / (float2(960,640) / 2);
	//		if (uv.x > 1.0 || uv.x < 0.0 ||
	//			uv.y > 1.0 || uv.y < 0.0) continue;
	//		uv *= float2(1.0, -1.0);
	//		float3 s = InputTexture.SampleLevel(samplerLinear, uv, 0);
	//		float c = 1.0;
	//		c *= 0.2;
	//		float3 value = max(0, s - float3(1,1,1) * c);
	//		bloom += lerp(value, 0, (abs(x) + abs(y)) / (numSamples * 2.0));
	//		++sampleCount;
	//	}
	//}
	//bloom /= sampleCount;
	//bloom *= 1.0;
	//col += max(0, bloom);

	return float4(col, 1.0);
}