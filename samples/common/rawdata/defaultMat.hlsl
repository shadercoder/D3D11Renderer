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
	float3 n = normalize(psIn.Normal);
	float3 l = normalize(float3(-1.0, 1.0, 1.0));
	float3 col = saturate(dot(normalize(l), n) * 1.0 / max(0.001, dot(l, l)));
	col = pow(col, 1.0 / 2.2);
	col *= float3(0.8, 0.4, 0.2);
	return float4(col, 1.0);
}