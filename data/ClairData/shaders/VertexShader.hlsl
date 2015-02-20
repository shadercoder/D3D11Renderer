cbuffer Buf : register(b0) {
	matrix World;
	matrix View;
	matrix Projection;
}

struct VSIn {
	float3 Position : POSITION;
	float2 Uvs : TEXCOORD;
};

struct VSOut {
	float4 Position : SV_POSITION;
	float2 Uvs : TEXCOORD;
};

VSOut main(VSIn vsIn) {
	VSOut vsOut;
	vsOut.Position = mul(Projection, mul(View, mul(World, float4(vsIn.Position, 1.0))));
	vsOut.Uvs = vsIn.Uvs;
	return vsOut;
}