cbuffer cbPerPass : register(b0)
{
	float4x4 view; 
	float4x4 proj; 
};
cbuffer cbPerObject : register(b2)
{
	float4x4 model; 
};

TextureCube textureDiffuse : register(t0);
SamplerState samplerDiffuse : register(s0);


struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 Pos  : SV_POSITION;
    float3 UV   : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.UV = vin.PosL;
	// 转换到齐次剪裁空间
    float4x4 view1 = view;
    view1[3].xyz = float3(0.0, 0.0, 0.0);
	float4 pos = mul(float4(vin.PosL, 1.0f), model);
	pos = mul(pos, view1);
	pos = mul(pos, proj);
    pos.w = pos.z;
    vout.Pos = pos;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return textureDiffuse.Sample(samplerDiffuse, pin.UV);
}
