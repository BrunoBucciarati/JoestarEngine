[maxvertexcount(4)] 
void GS(line VertexOutT gin[2],
    inout TriangleStream<GeoOut> triStream) 
{ 
    // Geometry shader body... 
} 
// 
// EXAMPLE 2: GS outputs at most 32 vertices.The input primitive is a triangle. The output is a triangle strip. 
// 
[maxvertexcount(32)] 
void GS(triangle VertexOut gin[3], 
inout TriangleStream<GeoOut> triStream) 
{ 
    // Geometry shader body... 
} 
// 
// EXAMPLE 3: GS outputs at most 4 vertices. The input primitive 
// is a point. The output is a triangle strip. 
// 
[maxvertexcount(4)] 
void GS(point VertexOut gin[1], 
inout TriangleStream<GeoOut> triStream) 
{ 
    // Geometry shader body... 
} 
