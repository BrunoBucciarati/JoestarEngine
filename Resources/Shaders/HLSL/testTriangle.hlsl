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
	vout.PosH = float4(vin.PosL, 1.0);
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return float4(0.0, 0.0, 1.0, 1.0);
}
