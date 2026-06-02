struct VertexOut{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 ps_main(VertexOut input) : SV_Target{
    float4 sampled = tex.Sample(samp, input.uv);

    return sampled;
}
