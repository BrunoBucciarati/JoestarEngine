cbuffer cbPerPass
{
	float4x4 view; 
	float4x4 proj; 
};
cbuffer cbPerObject
{
	float4x4 model; 
};

TextureCube gDiffTex : register(t0);
SamplerState gDiffSampler : register(s0);


struct VertexIn
{
	float3 PosL  : POSITION;
    float2 UV   : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float2 UV   : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{	
	// 转换到齐次剪裁空间
    float4x4 view1 = view;
    view1[3].xyz = float3(0.0, 0.0, 0.0);
	float4 pos = mul(float4(vin.PosL, 1.0f), model);
	pos = mul(pos, view1);
	pos = mul(pos, proj);
    pos.w = pos.z;
	vout.PosH = pos;
    vout.UV = UV;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return gDiffTex.Sample(gDiffSampler, pin.UV);
}
