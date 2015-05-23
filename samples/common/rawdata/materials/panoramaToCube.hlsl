Texture2D PanoramaInput : register(t0);
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
	psIn.Uvs = vsIn.Position.xy * 2.0 - 1.0;
	return psIn;
}

// -----------------------------------------------------------------------------
// PIXEL SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b1) {
	float Roughness;
};

struct PsOut {
	float4 Face0 : SV_TARGET0;
	float4 Face1 : SV_TARGET1;
	float4 Face2 : SV_TARGET2;
	float4 Face3 : SV_TARGET3;
	float4 Face4 : SV_TARGET4;
	float4 Face5 : SV_TARGET5;
};

static const float PI = 3.14159265358979;
float3 panoramaToCube(float2 uv, float3 right, float3 up, float3 forward) {
	float3 dir = normalize(forward + uv.x * right + uv.y * up);
	float u = fmod(0.5 * (1.0 + atan2(dir.x, -dir.z) * (1.0 / PI)) + 0.0, 1.0);
	float v = saturate(fmod(acos(dir.y) * (1.0 / PI), 1.0));
	float3 col = PanoramaInput.Sample(SamplerLinear, float2(1 - u, v)).rgb;
	return col;
}

PsOut psMain(PsIn psIn) {
	PsOut psOut;
	const float3 px = float3(1,0,0);
	const float3 nx = float3(-1,0,0);
	const float3 py = float3(0,1,0);
	const float3 ny = float3(0,-1,0);
	const float3 pz = float3(0,0,1);
	const float3 nz = float3(0,0,-1);
	psOut.Face0 = float4(panoramaToCube(psIn.Uvs, nz, py, px), 1.0);
	psOut.Face1 = float4(panoramaToCube(psIn.Uvs, pz, py, nx), 1.0);
	psOut.Face2 = float4(panoramaToCube(psIn.Uvs, px, nz, py), 1.0);
	psOut.Face3 = float4(panoramaToCube(psIn.Uvs, px, pz, ny), 1.0);
	psOut.Face4 = float4(panoramaToCube(psIn.Uvs, px, py, pz), 1.0);
	psOut.Face5 = float4(panoramaToCube(psIn.Uvs, nx, py, nz), 1.0);
	return psOut;
}