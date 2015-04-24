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
};

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
	float4 DiffuseColor;
}

struct PsOut {
	float4 Albedo : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Position : SV_TARGET2;
};

PsOut psMain(PsIn psIn) {
	PsOut psOut;
	psOut.Albedo = float4(DiffuseColor.xyz, 1.0);
	psOut.Normal = float4(normalize(psIn.Normal), 1.0);
	psOut.Position = float4(psIn.WorldPos / 10.0, 1.0);
	return psOut;
}