#include <metal_stdlib>

using namespace metal;

struct VertexOut{
    float4 position [[position]];
    float2 texCoord;
};

// TriangleStrip, Clockwise to Front-Face
static constant float2 positions[4] = {
    float2(-0.5,  0.5), float2(0.5,  0.5),
    float2(-0.5, -0.5), float2(0.5, -0.5)
};
static constant float2 texCoords[4] = {
    float2(0, 0), float2(1, 0),
    float2(0, 1), float2(1, 1)
};

vertex VertexOut vs_fullscreen(
    uint vertexID [[vertex_id]]
){
    VertexOut output;
    output.position = float4(positions[vertexID], 0, 1);
    output.texCoord = texCoords[vertexID];

    return output;
}
