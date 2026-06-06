#include <metal_stdlib>

using namespace metal;

struct VertexOut{
    float4 position [[position]];
    float2 texCoord;
};

struct SpriteConstants{
    float4x4 mvp;
    float2 uvScale;
    float2 offset;
};

// TriangleStrip, Clockwise to Front-Face
static constant float2 positions[4] = {
    float2(-1,  1), float2(1,  1),
    float2(-1, -1), float2(1, -1)
};
static constant float2 texCoords[4] = {
    float2(0, 0), float2(1, 0),
    float2(0, 1), float2(1, 1)
};

vertex VertexOut vs_main(
    uint vertexID [[vertex_id]],
    constant SpriteConstants& spriteConstants [[buffer(0)]]
){
    VertexOut output;
    output.position = spriteConstants.mvp * float4(positions[vertexID], 0, 1);
    output.texCoord = spriteConstants.uvScale * (texCoords[vertexID] + spriteConstants.offset);

    return output;
}

fragment float4 fs_main(
    VertexOut in [[stage_in]],
    texture2d<float> tex [[texture(0)]],
    sampler samp [[sampler(0)]]
){
    float4 sampled = tex.sample(samp, in.texCoord);

    return sampled;
}
