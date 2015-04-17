struct VsIn {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

struct PsIn {
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float3 WorldPos : WORLD_POSITION;
};

// -----------------------------------------------------------------------------
// VERTEX SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b0) {
	matrix World;
	matrix View;
	matrix Projection;
}

PsIn vsMain(VsIn vsIn) {
	PsIn psIn;
	psIn.Position = mul(Projection, mul(View, mul(World, float4(vsIn.Position, 1.0))));
	psIn.WorldPos = mul(World, float4(vsIn.Position, 1.0)).xyz;
	psIn.Normal = vsIn.Normal;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Material : register(b1) {
	float Reflectivity;
	float Roughness;
	float Metalness;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float3 n = normalize(psIn.Normal);
	float3 l = normalize(float3(-1.0, 5.0, -2.0));
	float3 col = saturate(dot(normalize(l), n) * 1.0 / max(0.001, dot(l, l)));
	col *= float3(Reflectivity, Roughness, Metalness);
	col = pow(col, 1.0 / 2.2);
	return float4(col, 1.0);
}