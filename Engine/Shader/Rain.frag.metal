#include <metal_stdlib>

using namespace metal;

struct RainCB
{
    float elapsedTime;
    float aspect;
    // 0 = no rain, 1 = full downpour
    float intensity;
    // fall speed multiplier
    float speedFactor;
    // streak tint, e.g. (0.7, 0.8, 1.0)
    packed_float3 color;
    // wind effect (0 = straight down)
    float slant;
    uint inversion;
};

struct VertexOut{
    float4 position [[position]];
    float2 uv;
};

float hash11(float n) {
    return fract(sin(n * 78.233) * 43758.5453);
}

float rainLayer(
    constant RainCB& rainCB,
    float2 uv,
    float columns,
    float dropsPerCol,
    float speed,
    float thickness
) {
    // wind: shift x as a function of y
    uv.x += uv.y * rainCB.slant;

    float colX = uv.x * columns;
    float col = floor(colX);
    // 0..1 across this lane
    float colU = fract(colX);

    // per-lane random seed
    float rnd = hash11(col);

    // horizontal profile
    float center = 0.25 + 0.5 * rnd;
    float lineMask = smoothstep(thickness, 0.0, abs(colU - center));

    // vertical motion
    float speedVar = mix(0.75, 1.25, hash11(col + 7.13));
    float phase = rnd * 17.0;
    float y = uv.y * dropsPerCol
        - rainCB.elapsedTime * speed * rainCB.speedFactor * speedVar
        + phase;
    float dropU = fract(y); // 0..1 within one drop

    // streak shape
    float head = pow(saturate(dropU), 12.0);       // sharp leading head
    float tail = 0.25 * pow(saturate(dropU), 3.0); // faint motion-blur tail
    float streak = head + tail;

    // per-drop brightness variation + sparseness
    float bright = smoothstep(0.25, 1.0, hash11(col + 13.7));

    return lineMask * streak * bright;
}

fragment float4 ps_main(
    VertexOut input [[stage_in]],
    constant RainCB& rainCB [[buffer(0)]],
    texture2d<float> tex [[texture(0)]],
    sampler linearClamp [[sampler(0)]]
) {
    float3 sampled = tex.sample(linearClamp, input.uv).rgb;
    sampled = (rainCB.inversion != 0) ?
        1.0 - sampled :
        sampled;

    // aspect-correct uv
    float2 uv = input.uv;
    uv.x *= rainCB.aspect;

    float rain = 0.0;
    rain += rainLayer(rainCB, uv, 220.0, 12.0, 1.30, 0.1) * 0.35; // far
    rain += rainLayer(rainCB, uv, 130.0, 8.0, 1.80, 0.2) * 0.60;  // mid
    rain += rainLayer(rainCB, uv, 70.0, 5.0, 2.40, 0.3) * 1.00;   // near

    rain *= rainCB.intensity;

    // screen blend so streaks brighten without blowing out
    float3 streak = float3(rainCB.color) * rain;
    float3 outColor = saturate(sampled + streak);
    // float3 outColor = 1.0 - (1.0 - sampled) * (1.0 - streak);

    return float4(outColor, 1.0);
}
