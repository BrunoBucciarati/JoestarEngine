cbuffer cbPerPass : register(b0)
{
	float4x4 view; 
	float4x4 proj; 
};
cbuffer cbPerObject : register(b2)
{
	float4x4 model; 
};

struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// 转换到齐次剪裁空间
	float4 pos = mul(float4(vin.PosL, 1.0f), model);
	pos = mul(pos, view);
	pos = mul(pos, proj);
	vout.PosH = pos;
    
    return vout;
}

void PS(VertexOut pin)
{
    // return float4(0.0, 0.0, 0.0, 0.0);
}
