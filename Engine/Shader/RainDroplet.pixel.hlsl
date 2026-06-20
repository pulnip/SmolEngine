//==================================================================
// Procedural Screen-Space Water Droplets - rim / trace style
//------------------------------------------------------------------
// Fullscreen post-process. Scatters water beads on the screen with
// a bright rim, a small specular highlight, and a faint inner
// darkening. NO refraction - the scene shows through undistorted;
// this is just the "trace" left by droplets on glass.
//
// wetness drives accumulation: raise it 0 -> 1 and beads fade in
// one by one (great for walking into a rainy zone).
//==================================================================

float hash21(float2 p) {
    p = frac(p * float2(123.34, 345.45));
    p += dot(p, p + 34.345);
    return frac(p.x * p.y);
}

float2 hash22(float2 p) {
    float n = sin(dot(p, float2(41.0, 289.0)));
    return frac(float2(262144.0, 32768.0) * n);
}

cbuffer rainDropletParam : register(b0)
{
    // --- row 1 ---
    float  aspect;    // viewport width / height (keeps beads round)
    float  wetness;   // 0 = dry ... 1 = fully covered (accumulation)
    float  strength;  // 0..1 overall effect opacity (zone fade)
    float  density;   // grid cells down the screen (more = smaller beads)
    // --- row 2 ---
    float3 rimColor;  // highlight tint, e.g. (0.9, 0.95, 1.0)
    float  specPower; // specular tightness, e.g. 4 (higher = sharper)
    // --- row 3 ---
    // uv.y is DOWN, so upper-left light = normalize(-0.6, -0.8)
    float2 lightDir;  // screen-space dir toward light.
};

//------------------------------------------------------------------
// One layer of droplets on a square grid.
//   Returns: x = highlight (rim + specular), y = inner mask (for wet darkening)
//
// Scans the 3x3 neighbourhood so beads near a cell edge bleed
// smoothly into neighbours instead of getting clipped.
//------------------------------------------------------------------
float2 dropletLayer(
    float2 uv,
    float gridScale,
    float2 seedOffset
) {
    float2 g    = uv * gridScale;
    float2 cell = floor(g);
    float2 f    = frac(g);

    float highlight = 0.0;
    float innerMax  = 0.0;

    [unroll] for(int y = -1; y <= 1; ++y)
    [unroll] for(int x = -1; x <= 1; ++x)
    {
        float2 offset = float2(x, y);
        float2 cid    = cell + offset + seedOffset;

        // --- accumulation: each bead has a spawn threshold; it fades
        //     in as wetness rises past it ---
        float seed   = hash21(cid + 3.1);
        float appear = smoothstep(seed, seed + 0.08, wetness);
        if (appear <= 0.0) continue;

        // --- placement: jittered center + varied radius ---
        float2 jitter = hash22(cid);
        float2 center = offset + jitter;                 // in current-cell local space
        float  radius = lerp(0.16, 0.32, hash21(cid + 7.7))
                      * lerp(0.6, 1.0, appear);          // grows in as it appears

        float2 p = f - center;                           // pixel relative to bead center
        float  d = length(p);

        // inside mask (soft edge)
        float inside = smoothstep(radius, radius - 0.02, d);

        // --- rim ring near the edge ---
        float nd  = d / max(radius, 1e-4);               // 0 center .. 1 edge
        float rim = smoothstep(0.55, 1.0, nd) * inside;

        // light-direction shading: brighter rim toward the light
        float2 nrm = p / max(d, 1e-4);
        float  lit = dot(nrm, lightDir) * 0.5 + 0.5;   // 0 away .. 1 toward
        rim *= lerp(0.35, 1.0, lit);

        // --- specular dot, offset toward the light ---
        float2 specPos = lightDir * radius * 0.45;
        float  specD   = length(p - specPos);
        float  spec    = pow(saturate(1.0 - specD / (radius * 0.35)), specPower);

        float h = (rim * 1.0 + spec * 1.3) * appear;

        highlight = max(highlight, h);
        innerMax  = max(innerMax, inside * appear);
    }

    return float2(highlight, innerMax);
}

struct VertexOut {
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

float4 ps_main(VertexOut input) : SV_Target {
    // aspect-correct so beads stay round, not stretched
    float2 uv = input.uv;
    uv.x *= aspect;

    // two layers
    // big beads + denser small beads
    // (offset seeds so they don't line up)
    float2 big   = dropletLayer(uv, density,        float2( 0.0, 0.0));
    float2 small = dropletLayer(uv, density * 2.3,  float2(19.7, 7.3));

    float highlight = max(big.x, small.x * 0.7) * strength;
    float inner     = max(big.y, small.y * 0.7) * strength;

    // subtle "wet" darkening inside beads, then add rim + specular
    float darkening = lerp(1.0, 0.85, inner * 0.5);

    // use alpha channel for darkening factor
    // additive blend + multiplicative alpha blend
    return float4(rimColor * highlight, darkening);
}
