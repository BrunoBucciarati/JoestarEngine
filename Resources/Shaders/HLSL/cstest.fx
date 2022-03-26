cbuffer cbSettings
{
// Compute shader can access values in constant buffers.
};
// Data sources and outputs.
// Texture2D gInputA;
// Texture2D gInputB;
// RWTexture2D<float4> gOutput;
struct Data
{
    float3 v1;
};
StructuredBuffer<Data> gInputA;
RWStructuredBuffer<Data> gOutput;
// The number of threads in the thread group. The threads in a group can
// be arranged in a 1D, 2D, or 3D grid layout.
[numthreads(32, 1, 1)]
void CS(int3 dtid : SV_DispatchThreadID)
{
    gOutput[dtid.x].v1 = gInputA[dtid.x].v1 + float3(dtid.x, dtid.x, dtid.x );
}

