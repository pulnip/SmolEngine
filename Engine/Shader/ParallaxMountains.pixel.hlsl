float hash11(float f) {
    f = frac(f * 0.1031);
    f *= f + 33.33;
    f *= f + f;
    return frac(f);
}

float vnoise(float x) {
    float i = floor(x);
    float f = frac(x);
    float u = f * f * (3.0 - 2.0 * f); // smoothstep
    return lerp(hash11(i), hash11(i + 1.0), u);
}

#define OCTAVES 5

float ridgeFBM(float x) {
    float sum = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    [unroll]
    for(int i = 0; i < OCTAVES; ++i)
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

#define NUM_LAYERS 3

cbuffer mountainParam : register(b0)
{
    // Far to Near
    MountainLayer layers[NUM_LAYERS];
    // Sky Gradation
    float4 skyTop;
    float4 skyBottom;
    float elapsedTime;
};

float layerHeight(
    in MountainLayer L,
    float uvx,
    float seed
) {
    float x = uvx * L.frequency + seed + L.scrollSpeed * elapsedTime;
    return L.baseHeight + ridgeFBM(x) * L.amplitude;
}

float layerMask(float py, float ridgeH) {
    float aa = fwidth(ridgeH) + 0.0015;
    return 1.0 - smoothstep(ridgeH - aa, ridgeH + aa, py);
}

static const float SEEDS[NUM_LAYERS] = {
    11.3,
    53.7,
    91.1
};

struct VertexOut {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 ps_main(VertexOut input) : SV_Target {
    float py = 1.0 - input.uv.y; // bottom-origin
    float3 col = lerp(skyBottom.rgb, skyTop.rgb, py);

    [unroll]
    for (int i = 0; i < NUM_LAYERS; ++i)
    {
        float h = layerHeight(
            layers[i],
            input.uv.x,
            SEEDS[i]
        );
        col = lerp(col, layers[i].color.rgb, layerMask(py, h));
    }

    return float4(col, 1.0);
}
