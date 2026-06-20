#include <metal_stdlib>

using namespace metal;

float hash21(float2 p){
    p = fract(p * float2(123.34, 345.45));
    p += dot(p, p + 34.345);
    return fract(p.x * p.y);
}

float2 hash22(float2 p){
    float n = sin(dot(p, float2(41.0, 289.0)));
    return fract(float2(262144.0, 32768.0) * n);
}

struct RainDropletParam
{
    float aspect;
    // 0 = dry, 1 = fully covered
    float wetness;
    float strength;
    float density;
    packed_float3 rimColor;
    float specPower;
    float2 lightDir;
};

float2 dropletLayer(
    constant RainDropletParam& param,
    float2 uv,
    float gridScale,
    float2 seedOffset
){
    float2 g = uv * gridScale;
    float2 cell = floor(g);
    float2 f = fract(g);

    float highlight = 0.0;
    float innerMax = 0.0;

    [[unroll]] for(int y = -1; y <= 1; ++y)
    [[unroll]] for(int x = -1; x <= 1; ++x)
    {
        float2 offset = float2(x, y);
        float2 cid = cell + offset + seedOffset;

        float seed = hash21(cid + 3.1);
        float appear = smoothstep(seed, seed+0.08, param.wetness);
        if(appear <= 0.0)
            continue;

        float2 jitter = hash22(cid);
        float2 center = offset + jitter;
        float radius = mix(0.16, 0.32, hash21(cid + 7.7)) * mix(0.6, 1.0, appear);

        float2 p = f - center;
        float d = length(p);

        float inside = smoothstep(radius, radius - 0.02, d);

        float nd = d / max(radius, 1e-4);
        float rim = smoothstep(0.55, 1.0, nd) * inside;

        float2 nrm = p / max(d, 1e-4);
        float lit = dot(nrm, param.lightDir) * 0.5 + 0.5;
        rim *= mix(0.35, 1.0, lit);

        float2 specPos = param.lightDir * radius * 0.45;
        float specD = length(p - specPos);
        float spec = pow(saturate(1.0 - specD / (radius * 0.35)), param.specPower);

        float h = (rim * 1.0 + spec * 1.3) * appear;

        highlight = max(highlight, h);
        innerMax = max(innerMax, inside * appear);
    }

    return float2(highlight, innerMax);
}

struct VertexOut{
    float4 position [[position]];
    float2 uv;
};

fragment float4 fs_main(
    VertexOut input [[stage_in]],
    constant RainDropletParam& rainDropletParam [[buffer(0)]]
){
    // aspect-correct uv
    float2 uv = input.uv;
    uv.x *= rainDropletParam.aspect;

    // big beads + denser small beads
    float2   big = dropletLayer(rainDropletParam, uv,       rainDropletParam.density, float2( 0.0, 0.0));
    float2 small = dropletLayer(rainDropletParam, uv, 2.3 * rainDropletParam.density, float2(19.7, 7.3));

    float highlight = max(big.x, 0.7 * small.x) * rainDropletParam.strength;
    float inner = max(big.y, 0.7 * small.y) * rainDropletParam.strength;

    float darkening = mix(1.0, 0.85, inner * 0.5);

    return float4(rainDropletParam.rimColor * highlight, darkening);
}