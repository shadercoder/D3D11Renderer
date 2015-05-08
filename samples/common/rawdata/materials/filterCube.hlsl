TextureCube CubeMapInput : register(t0);
SamplerState SamplerLinear : register(s0);

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
	psIn.Uvs = vsIn.Position.xy;// * 2.0 - 1.0;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
//cbuffer Buf : register(b1) {
//	float4 FinalColor;
//};

struct PsOut {
	float4 Face0 : SV_TARGET0;
	float4 Face1 : SV_TARGET1;
	float4 Face2 : SV_TARGET2;
	float4 Face3 : SV_TARGET3;
	float4 Face4 : SV_TARGET4;
	float4 Face5 : SV_TARGET5;
};

PsOut psMain(PsIn psIn) {
	PsOut psOut;
	float3 col;
	col = CubeMapInput.Sample(SamplerLinear, float3(psIn.Uvs, 1.0)).rgb;
	//col *= float3(1.0, 1.0, 0.0);
	float4 finalCol = float4(col, 1.0);
	psOut.Face0 = finalCol;
	psOut.Face1 = finalCol;
	psOut.Face2 = finalCol;
	psOut.Face3 = finalCol;
	psOut.Face4 = finalCol;
	psOut.Face5 = finalCol;
	return psOut;
}