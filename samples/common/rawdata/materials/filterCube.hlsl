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

// Hammersley from
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float radicalInverse_VdC(uint bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
float2 hammersley2d(uint i, uint N) {
	return float2(float(i)/float(N), radicalInverse_VdC(i));
}
static const float PI = 3.14159265358979;
float3 hemisphereSample_uniform(float2 uv) {
	float phi = uv.y * 2.0 * PI;
	float cosTheta = 1.0 - uv.x;
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

float3 filter(float2 uv, float3 right, float3 up, float3 forward) {
	float3 col = float3(0, 0, 0);
	float3 dir = normalize(forward + uv.x * right + uv.y * up);
	const int NUM_SAMPLES = 128;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		float sign = i % 2 == 0 ? 1.0f : -1.0f;
		float3 offset = hemisphereSample_uniform(hammersley2d(i, NUM_SAMPLES));
		float3 sampDir = normalize(dir + 0.8 * Roughness * sign * offset);
		col += pow(CubeMapInput.Sample(SamplerLinear, sampDir).rgb, 2.2);
	}
	col /= float(NUM_SAMPLES);
	return pow(col, 1.0 / 2.2);
}

PsOut psMain(PsIn psIn) {
	PsOut psOut;
	float3 col;
	col = CubeMapInput.Sample(SamplerLinear, float3(psIn.Uvs, 1.0)).rgb;
	float4 finalCol = float4(col, 1.0);
	const float3 px = float3(1,0,0);
	const float3 nx = float3(-1,0,0);
	const float3 py = float3(0,1,0);
	const float3 ny = float3(0,-1,0);
	const float3 pz = float3(0,0,1);
	const float3 nz = float3(0,0,-1);
	psOut.Face0 = float4(filter(psIn.Uvs, nz, py, px), 1.0);
	psOut.Face1 = float4(filter(psIn.Uvs, pz, py, nx), 1.0);
	psOut.Face2 = float4(filter(psIn.Uvs, px, nz, py), 1.0);
	psOut.Face3 = float4(filter(psIn.Uvs, px, pz, ny), 1.0);
	psOut.Face4 = float4(filter(psIn.Uvs, px, py, pz), 1.0);
	psOut.Face5 = float4(filter(psIn.Uvs, nx, py, nz), 1.0);
	return psOut;
}