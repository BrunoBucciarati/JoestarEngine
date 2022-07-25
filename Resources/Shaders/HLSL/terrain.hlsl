cbuffer cbPerPass : register(b0)
{
	float4x4 view; 
	float4x4 proj; 
	float4 cameraPos;
};

cbuffer cbPerObject : register(b2)
{
	float4 frustumPlanes[6];
};

// Texture2D textureDiffuse : register(t0);
// SamplerState samplerDiffuse : register(s0);
Texture2D textureHeightMap : register(t1);
SamplerState samplerHeightMap : register(s1);


struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Nor : NORMAL;
	float2 UV : TEXCOORD0;
	float2 BoundsY : TEXCOORD1;
};

struct VertexOut
{
	float3 PosW  : POSITION;
	float3 Nor : NORMAL;
	float2 Tex : TEXCOORD0;
	float2 BoundsY : TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosW = vin.PosL;
	vout.PosW.y = textureHeightMap.SampleLevel(samplerHeightMap, vin.UV, 0).r;
	vout.Nor = vin.Nor;
	vout.Tex = vin.UV;
	vout.BoundsY = vin.BoundsY;
    
    return vout;
}

float CalcTessFactor(float3 p)
{
	// When distance is minimum, the tessellation is maximum.
	// When distance is maximum, the tessellation is minimum.
	float gMinDist = 1.0;
	float gMaxDist = 100.0;
	// Exponents for power of 2 tessellation. The tessellation
	// range is [2^(gMinTess), 2^(gMaxTess)]. Since the maximum
	// tessellation is 64, this means gMaxTess can be at most 6
	// since 2^6 = 64, and gMinTess must be at least 0 since 2^0 = 1.
	float gMinTess = 1.0;
	float gMaxTess = 16.0;

	// float d = distance(p, cameraPos.xyz);
	float3 dist = p - cameraPos.xyz; 
	float d = sqrt(dot(dist, dist));
	float s = saturate((d - gMinDist) / (gMaxDist - gMinDist));
	s = 1.0 - pow(1.0 - s, 4.0);
	float fac = s * gMinTess + (1.0 - s) * gMaxTess;

	return fac;
}

struct PatchTess
{
	float EdgeTess[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

bool AABBBehindPlaneTest(float3 center, float3 extents, float4 plane)
{
	float3 n = abs(plane.xyz); // (|n.x|, |n.y|, |n.z|)
	float e = plane.w;
	// This is always positive.
	float r = dot(extents, n);
	// signed distance from center point to plane.
	float s = dot(float4(center, 1.0f), plane);
	// If the center point of the box is a distance of e or more behind the
	// plane (in which case s is negative since it is behind the plane),
	// then the box is completely in the negative half space of the plane.

	//todo 平面那边可能数据还没组织好，不过逻辑已有，后续可加
	return false;//(s + e) < 0.0f;
}

// Returns true if the box is completely outside the frustum.
bool AABBOutsideFrustumTest(float3 center, float3 extents, float4 frustumPlanes[6])
{
	for(int i = 0; i < 6; ++i)
	{
		// If the box is completely behind any of the frustum planes
		// then it is outside the frustum.
		if(AABBBehindPlaneTest(center, extents, frustumPlanes[i]))
		{
			return true;
		}
	}
	return false;
}


PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;
	//
	float minY = patch[0].BoundsY.x;
	float maxY = patch[0].BoundsY.y;
	// Build axis-aligned bounding box. patch[2] is lower-left corner
	// and patch[1] is upper-right corner.
	float3 vMin = float3(patch[2].PosW.x, minY, patch[2].PosW.z);
	float3 vMax = float3(patch[1].PosW.x, maxY, patch[1].PosW.z);
	// Center/extents representation.
	float3 boxCenter = 0.5f*(vMin + vMax);
	float3 boxExtents = 0.5f*(vMax - vMin);
	if(false)//AABBOutsideFrustumTest(boxCenter, boxExtents, frustumPlanes))
	{
		pt.EdgeTess[0] = 0.0f;
		pt.EdgeTess[1] = 0.0f;
		pt.EdgeTess[2] = 0.0f;
		pt.EdgeTess[3] = 0.0f;
		pt.InsideTess[0] = 0.0f;
		pt.InsideTess[1] = 0.0f;
		return pt;
	}
	else
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
[partitioning("fractional_even")]
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
