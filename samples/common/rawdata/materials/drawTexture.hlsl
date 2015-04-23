Texture2D texAlbedo : register(t0);
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
float4 psMain(PsIn psIn) : SV_TARGET {
	float3 col = texAlbedo.Sample(samplerLinear, psIn.Uvs * float2(1.0, -1.0));
	if (psIn.Uvs.x > 0) col.b += 0.5;
	if (psIn.Uvs.y > 0) col.g += 0.5;
	return float4(col, 1.0);
}