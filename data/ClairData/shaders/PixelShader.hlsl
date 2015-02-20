Texture2D texAlbedo : register(t0);
SamplerState samplerLinear : register(s0);

struct VSOut {
	float4 Position : SV_POSITION;
	float2 Uvs : TEXCOORD;
};

float4 main(VSOut psIn) : SV_TARGET {
    return texAlbedo.Sample(samplerLinear, psIn.Uvs);
	return float4(1.0, 0.2, 0.3, 1.0);
}