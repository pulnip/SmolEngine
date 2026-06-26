#include <metal_stdlib>

using namespace metal;

float hash11(float f) {
    f = fract(f * 0.1031);
    f *= f + 33.33;
    f *= f + f;
    return fract(f);
}

float vnoise(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f); // smoothstep
    return mix(hash11(i), hash11(i + 1.0), u);
}

#define OCTAVES 5

float ridgeFBM(float x) {
    float sum = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    [[unroll]] for(int i = 0; i < OCTAVES; ++i)
    {
        float n = vnoise(x * freq);
        n = 1.0 - abs(n * 2.0 - 1.0);
        n *= n; // sharpening peak
        sum += n * amp;
        freq *= 2.0; // lacunarity
        amp *= 0.5;  // gain
    }
    return sum;
}

struct MountainLayer {
    float scrollSpeed;
    float baseHeight;
    float amplitude;
    float frequency;
    float4 color;
};

float layerHeight(
    constant MountainLayer& L,
    float uvx,
    float seed,
    float elapsedTime
) {
    float x = uvx * L.frequency + seed + L.scrollSpeed * elapsedTime;
    return L.baseHeight + ridgeFBM(x) * L.amplitude;
}

float layerMask(float py, float ridgeH) {
    float aa = fwidth(ridgeH) + 0.0015;
    return 1.0 - smoothstep(ridgeH - aa, ridgeH + aa, py);
}

#define NUM_LAYERS 3

struct MountainParam{
    // Far to Near
    MountainLayer layers[NUM_LAYERS];
    // Sky Gradation
    float4 skyTop;
    float4 skyBottom;
    float elapsedTime;
};

static constant float SEEDS[NUM_LAYERS] = {
    11.3,
    53.7,
    91.1
};

struct VertexOut{
    float4 position [[position]];
    float2 uv;
};

fragment float4 fs_main(
    VertexOut input [[stage_in]],
    constant MountainParam& mountainParam [[buffer(0)]]
){
    float py = 1.0 - input.uv.y; // bottom-origin
    float3 col = mix(
        mountainParam.skyBottom.rgb,
        mountainParam.skyTop.rgb,
        py
    );

    [[unroll]] for(int i = 0; i < NUM_LAYERS; ++i)
    {
        float h = layerHeight(
            mountainParam.layers[i],
            input.uv.x,
            SEEDS[i],
            mountainParam.elapsedTime
        );
        col = mix(
            col,
            mountainParam.layers[i].color.rgb,
            layerMask(py, h)
        );
    }

    return float4(col, 1.0);

}
