TextureCube CubeMap : register(t0);
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

struct PsOut {
	float2 RT1 : SV_TARGET0;
	float4 RT2 : SV_TARGET1;
	float4 RT3 : SV_TARGET2;
};

PsOut psMain(PsIn psIn) {
	PsOut psOut;
	float dist = 1.0 / tan(FieldOfView / 2.0 * 3.14 / 180.0);
	float3 r = CamForward * dist +
			   (CamRight * psIn.Uvs.x * Aspect) +
			   (CamUp * psIn.Uvs.y);
	r = normalize(r);
	float3 col = CubeMap.SampleLevel(samplerLinear, r, 0);

	psOut.RT1 = float2(0.0, 0.0);
	psOut.RT2 = float4(col, 1.0);
	psOut.RT3 = float4(0.0, 0.0, 0.0, 0.0);
	return psOut;
}