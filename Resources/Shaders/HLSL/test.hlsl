cbuffer cbPerPass : register(b0)
{
	float4x4 view; 
	float4x4 proj; 
	float4 cameraPos;
};
cbuffer cbPerObject : register(b2)
{
	float4x4 model; 
};

Texture2D textureDiffuse : register(t0);
SamplerState samplerDiffuse : register(s0);


struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Nor : NORMAL;
	float2 UV : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float2 UV : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// 转换到齐次剪裁空间
	float4 pos = mul(float4(vin.PosL, 1.0f), model);
	pos = mul(pos, view);
	pos = mul(pos, proj);
	vout.PosH = pos;
	
	// 将顶点颜色直接传递到像素着色器
    vout.UV = vin.UV;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return textureDiffuse.Sample(samplerDiffuse, pin.UV);
}
