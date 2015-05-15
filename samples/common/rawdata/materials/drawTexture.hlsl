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
	float3 col = InputTexture.Sample(samplerLinear, psIn.Uvs * float2(1.0, -1.0));
	return float4(col, 1.0);
}