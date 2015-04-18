Texture2D texAlbedo : register(t0);
SamplerState samplerLinear : register(s0);

struct VsIn {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

struct PsIn {
	float4 Position : SV_POSITION;
	float3 ViewNormal : NORMAL;
	float3 ViewPos : VIEW_POSITION;
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
	psIn.ViewPos = mul(View, mul(World, float4(vsIn.Position, 1.0))).xyz;
	psIn.ViewNormal = mul(View, mul(World, float4(vsIn.Normal, 0.0))).xyz;
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
	float3 n = normalize(psIn.ViewNormal);
	float3 l = normalize(mul(View, float4(-1.0, 5.0, -2.0, 0.0))).xyz;
	float3 viewVec = normalize(psIn.ViewPos);
	float3 refl = reflect(viewVec, n);
	float3 col = saturate(dot(normalize(l), n) * 1.0 / max(0.001, dot(l, l)));
	col *= float3(Reflectivity, Roughness, Metalness);
	//col *= texAlbedo.Sample(samplerLinear, refl.xz).rgb;
	col = pow(col, 1.0 / 2.2);
	return float4(col, 1.0);
}