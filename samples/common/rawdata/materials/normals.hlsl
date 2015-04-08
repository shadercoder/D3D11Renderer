// -----------------------------------------------------------------------------
// STRUCTS
// -----------------------------------------------------------------------------
cbuffer Buf : register(b0) {
	matrix World;
	matrix View;
	matrix Projection;
}

Texture2D texAlbedo : register(t0);
SamplerState samplerLinear : register(s0);

struct VSIn {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

struct VSOut {
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float3 WorldPos : WORLD_POSITION;
};

// -----------------------------------------------------------------------------
// VERTEX SHADER
// -----------------------------------------------------------------------------
VSOut vsMain(VSIn vsIn) {
	VSOut vsOut;
	vsOut.Position = mul(Projection, mul(View, mul(World, float4(vsIn.Position, 1.0))));
	vsOut.WorldPos = mul(World, float4(vsIn.Position, 1.0)).xyz;
	vsOut.Normal = vsIn.Normal;
	return vsOut;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
float4 psMain(VSOut psIn) : SV_TARGET {
	return float4(normalize(psIn.Normal), 1.0);
}