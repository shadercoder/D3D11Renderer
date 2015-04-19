Texture2D texAlbedo : register(t0);
SamplerState samplerLinear : register(s0);

struct VsIn {
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

struct PsIn {
	float4 Position : SV_POSITION;
	float3 WPosition : WPOSITION;
	float3 WNormal : WNORMAL;
};

// -----------------------------------------------------------------------------
// VERTEX SHADER
// -----------------------------------------------------------------------------
cbuffer Buf : register(b0) {
	matrix World;
	matrix View;
	matrix Projection;
	float3 CameraPos;
}

PsIn vsMain(VsIn vsIn) {
	PsIn psIn;
	psIn.Position =
			mul(Projection, mul(View, mul(World, float4(vsIn.Position, 1.0))));
	psIn.WPosition = mul(World, float4(vsIn.Position, 1.0)).xyz;
	psIn.WNormal = mul(World, float4(vsIn.Normal, 0.0)).xyz;
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
	float3 n = normalize(psIn.WNormal);
	float3 l = normalize(float3(-1.0, 5.0, -2.0));
	float3 viewVec = normalize(psIn.WPosition - CameraPos);
	float3 refl = reflect(viewVec, n);
	float3 albedo = float3(1.0, 0.0, 0.0);
	//float3(Reflectivity, Roughness, Metalness);
	float diff = saturate(dot(l, n) * 1.0 / max(0.001, dot(l, l)));
	float3 reflCol =
		texAlbedo.Sample(samplerLinear, (refl.xz / refl.y) * 0.4).rgb;
	float3 col = albedo * diff;
	col = pow(col, 1.0 / 2.2);
	col = lerp(col, reflCol, Reflectivity);
	return float4(col, 1.0);
}