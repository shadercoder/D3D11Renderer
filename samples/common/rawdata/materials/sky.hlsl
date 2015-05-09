TextureCube texAlbedo : register(t0);
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
cbuffer Material : register(b1) {
	float3 CamRight;
	float3 CamUp;
	float3 CamForward;
	float Aspect;
	float FieldOfView;
}

float4 psMain(PsIn psIn) : SV_TARGET {
	float dist = 1.0 / tan(FieldOfView / 2.0 * 3.14 / 180.0);
	float3 r = CamForward * dist +
			   (CamRight * psIn.Uvs.x * Aspect) +
			   (CamUp * psIn.Uvs.y);
	r = normalize(r);
	float3 col = texAlbedo.SampleLevel(samplerLinear, r, 0);
	col = pow(col, 1.0 / 2.2);
	return float4(col, 1.0);
}