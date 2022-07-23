cbuffer cbPerPass : register(b0)
{
	float4x4 view; 
	float4x4 proj; 
};

Texture2D textureDiffuse : register(t0);
SamplerState samplerDiffuse : register(s0);
Texture2D textureHeightMap : register(t1);
SamplerState samplerHeightMap : register(s1);


struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Nor : NORMAL;
	float2 UV : TEXCOORD0;
};

struct VertexOut
{
	float3 PosW  : POSITION;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosW = vin.PosL;
	vout.PosW.y = textureHeightMap.SampleLevel(samplerHeightMap, vin.UV, 0).r;
	vout.Nor = vin.Nor;
	vout.Tex = vin.UV;
    
    return vout;
}

float CalcTessFactor(float3 p)
{
// When distance is minimum, the tessellation is maximum.
// When distance is maximum, the tessellation is minimum.
	float gMinDist = 1.0;
	float gMaxDist = 1000.0;
// Exponents for power of 2 tessellation. The tessellation
// range is [2^(gMinTess), 2^(gMaxTess)]. Since the maximum
// tessellation is 64, this means gMaxTess can be at most 6
// since 2^6 = 64, and gMinTess must be at least 0 since 2^0 = 1.
	float gMinTess = 1.0;
	float gMaxTess = 64.0;
	float gEyePosW = 1.0;

	float d = distance(p, gEyePosW);
	float s = saturate((d - gMinDist) / (gMaxDist - gMinDist));
	return pow(2, (lerp(gMaxTess, gMinTess, s)));
}

struct PatchTess
{
	float EdgeTess[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;
	//
	// Frustum cull
	//
	// [... Omit frustum culling code]
	// //
	// // Do normal tessellation based on distance.
	// //
	// else
	{
		// It is important to do the tess factor calculation
		// based on the edge properties so that edges shared
		// by more than one patch will have the same
		// tessellation factor. Otherwise, gaps can appear.
		// Compute midpoint on edges, and patch center
		float3 e0 = 0.5f*(patch[0].PosW + patch[2].PosW);
		float3 e1 = 0.5f*(patch[0].PosW + patch[1].PosW);
		float3 e2 = 0.5f*(patch[1].PosW + patch[3].PosW);
		float3 e3 = 0.5f*(patch[2].PosW + patch[3].PosW);
		float3 c = 0.25f*(patch[0].PosW + patch[1].PosW + patch[2].PosW + patch[3].PosW);
		pt.EdgeTess[0] = CalcTessFactor(e0);
		pt.EdgeTess[1] = CalcTessFactor(e1);
		pt.EdgeTess[2] = CalcTessFactor(e2);
		pt.EdgeTess[3] = CalcTessFactor(e3);
		pt.InsideTess[0] = CalcTessFactor(c);
		pt.InsideTess[1] = pt.InsideTess[0];
		return pt;
	}
}

struct HullOut
{
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD0;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 4> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	HullOut hout;
	hout.PosW = p[i].PosW;
	hout.Tex = p[i].Tex;
	return hout;
}


struct DomainOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD0;
	float2 TiledTex : TEXCOORD1;
};

[domain("quad")]
DomainOut DS(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
	// How much to tile the texture layers.
	float2 gTexScale = 50.0f;

	DomainOut dout;
	// Bilinear interpolation.
	dout.PosW = lerp(
		lerp(quad[0].PosW, quad[1].PosW, uv.x),
		lerp(quad[2].PosW, quad[3].PosW, uv.x),
		uv.y);
	dout.Tex = lerp(
		lerp(quad[0].Tex, quad[1].Tex, uv.x),
		lerp(quad[2].Tex, quad[3].Tex, uv.x),
		uv.y);
	// Tile layer textures over terrain.
	dout.TiledTex = dout.Tex*gTexScale;
	// Displacement mapping
	dout.PosW.y = textureHeightMap.SampleLevel(samplerHeightMap, dout.Tex, 0).r;
	// NOTE: We tried computing the normal in the domain shader
	// using finite difference, but the vertices move continuously
	// with fractional_even which creates noticable light shimmering
	// artifacts as the normal changes. Therefore, we moved the
	// calculation to the pixel shader.
	// Project to homogeneous clip space.
	float4 viewPos = mul(float4(dout.PosW, 1.0f), view);
	dout.PosH = mul(viewPos, proj);
	return dout;
}


float4 PS(DomainOut pin) : SV_Target
{
    return float4(1.0, 1.0, 1.0, 1.0);
}
