struct VertexOut{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer spriteConstants : register(b0)
{
    float4x4 mvp;
    float2 uvScale;
    float2 offset;
};

VertexOut vs_main(uint vertexID : SV_VertexID){
    // TriangleStrip, Clockwise to Front-Face
    static const float2 ndc[4] = {
        float2(-1,  1), float2(1,  1),
        float2(-1, -1), float2(1, -1)
    };
    static const float2 texCoords[4] = {
        float2(0, 0), float2(1, 0),
        float2(0, 1), float2(1, 1)
    };

    VertexOut output;
    output.position = mul(mvp, float4(ndc[vertexID], 0, 1));
    output.uv = uvScale * (texCoords[vertexID] + offset);

    return output;
}
