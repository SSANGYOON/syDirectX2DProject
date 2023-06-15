cbuffer Transform : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix projection;
    int entity;
    float3 padding;
}

cbuffer Material : register(b1)
{
    int     tex0_On;
    int     tex1_On;
    int     tex2_On;
    int     tex3_On;

    int     g_int_0;
    int     g_int_1;
    int     g_int_2;
    int     g_int_3;

    float   g_float_0;
    float   g_float_1;
    float   g_float_2;
    float   g_float_3;

    float2  g_vec2_0;
    float2  g_vec2_1;
    float2  g_vec2_2;
    float2  g_vec2_3;

    float2  g_tex0_size;
    float2  g_tex1_size;
    float2  g_tex2_size;
    float2  g_tex3_size;

    float4  g_vec4_0;
    float4  g_vec4_1;
    float4  g_vec4_2;
    float4  g_vec4_3;
}

struct LightInfo
{
    float3 dir;
    float angle;
    float3 position;
    float range;
    float3 color;
    uint type;
};

cbuffer VisualEffect : register(b2)
{
    float time;
    float DeltaTime;
    float2 viewPort;
    float4 ScissorRect;
    float4 FadeColor;
}

cbuffer Lights : register(b3)
{
    uint         g_lightCount;
    float3      g_lightPadding;
    LightInfo   g_light[50];
}

cbuffer ParticleBuffer : register(b4)
{
    float2  LocalPosition;
    float2  PositionVariation;

    float2  Velocity;
    float2  VelocityVariation;
    float2  VelocityEnd;

    float2 SizeBegin;
    float2 SizeEnd;
    float2 SizeVariation;

    float4 ColorBegin;
    float4 ColorEnd;

    float4 EmissionBegin;
    float4 EmissionEnd;

    row_major float4x4 worldTrans;

    float LifeTime;
    uint addCount;
    uint PositionPolar;
    uint VelocityPolar;

    uint maxParticles;
    uint textureAttach;
    float2 targetTexturePos;

    uint useLocalCoord;
    uint useAliveZone;
    float2 aliveZone;
};

cbuffer BlurBuffer : register(b5)
{
    float extractThreshold;
    float bloomIntensity;
    float2 ImgSize;
};

SamplerState pointSampler : register(s0);
SamplerState linearSampler : register(s1);
SamplerState anisotropicSampler : register(s2);

Texture2D tex_0 : register(t0);
Texture2D tex_1 : register(t1);
Texture2D tex_2 : register(t2);
Texture2D tex_3 : register(t3);

Texture2D Position : register(t2);

float Rand(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

float3 CalculateLight(float3 position, float lightmap)
{
    float3 ret = (float3)0.f;

    for (uint idx = 0; idx < g_lightCount; idx++) {
        if (0 == g_light[idx].type)
        {
            float dis = distance(position, g_light[idx].position);

            if (dis < g_light[idx].range)
                ret += g_light[idx].color * pow(1 - dis / g_light[idx].range, 2) * lightmap;
        }
        else if (1 == g_light[idx].type)
        {
            float3 diff = position - g_light[idx].position;

            float proj = dot(diff, g_light[idx].dir);

            if (proj > 0 && proj > length(diff) * cos(g_light[idx].angle) && proj < g_light[idx].range)
                ret += g_light[idx].color * pow(1 - proj / g_light[idx].range,2) * lightmap;
        }

        else if (2 == g_light[idx].type)
        {
            ret += g_light[idx].color * lightmap;
        }
        else
        {
            ret += g_light[idx].color;
        }
    }

    return ret;
}

inline float unity_noise_randomValue(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

inline float unity_noise_interpolate(float a, float b, float t)
{
    return (1.0 - t) * a + (t * b);
}

inline float unity_valueNoise(float2 uv)
{
    float2 i = floor(uv);
    float2 f = frac(uv);
    f = f * f * (3.0 - 2.0 * f);

    uv = abs(frac(uv) - 0.5);
    float2 c0 = i + float2(0.0, 0.0);
    float2 c1 = i + float2(1.0, 0.0);
    float2 c2 = i + float2(0.0, 1.0);
    float2 c3 = i + float2(1.0, 1.0);
    float r0 = unity_noise_randomValue(c0);
    float r1 = unity_noise_randomValue(c1);
    float r2 = unity_noise_randomValue(c2);
    float r3 = unity_noise_randomValue(c3);

    float bottomOfGrid = unity_noise_interpolate(r0, r1, f.x);
    float topOfGrid = unity_noise_interpolate(r2, r3, f.x);
    float t = unity_noise_interpolate(bottomOfGrid, topOfGrid, f.y);
    return t;
}

float Unity_SimpleNoise_float(float2 UV, float Scale)
{
    float t = 0.0;

    float freq = pow(2.0, float(0));
    float amp = pow(0.5, float(3 - 0));
    t += unity_valueNoise(float2(UV.x * Scale / freq, UV.y * Scale / freq)) * amp;

    freq = pow(2.0, float(1));
    amp = pow(0.5, float(3 - 1));
    t += unity_valueNoise(float2(UV.x * Scale / freq, UV.y * Scale / freq)) * amp;

    freq = pow(2.0, float(2));
    amp = pow(0.5, float(3 - 2));
    t += unity_valueNoise(float2(UV.x * Scale / freq, UV.y * Scale / freq)) * amp;

    return t;
}

void Unity_TilingAndOffset_float(float2 UV, float2 Tiling, float2 Offset, out float2 Out)
{
    Out = UV * Tiling + Offset;
}

inline float2 unity_voronoi_noise_randomVector(float2 UV, float offset)
{
    float2x2 m = float2x2(15.27, 47.63, 99.41, 89.98);
    UV = frac(sin(mul(UV, m)) * 46839.32);
    return float2(sin(UV.y * +offset) * 0.5 + 0.5, cos(UV.x * offset) * 0.5 + 0.5);
}

void Unity_Voronoi_float(float2 UV, float AngleOffset, float CellDensity, out float Out, out float Cells)
{
    float2 g = floor(UV * CellDensity);
    float2 f = frac(UV * CellDensity);
    float t = 8.0;
    float3 res = float3(8.0, 0.0, 0.0);

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 lattice = float2(x, y);
            float2 offset = unity_voronoi_noise_randomVector(lattice + g, AngleOffset);
            float d = distance(lattice + offset, f);
            if (d < res.x)
            {
                res = float3(d, offset.x, offset.y);
                Out = res.x;
                Cells = res.y;
            }
        }
    }
}

float Unity_Rectangle_float(float2 UV, float Width, float Height)
{
    float2 d = abs(UV * 2 - 1) - float2(Width, Height);
    d = 1 - d / fwidth(d);
    return saturate(min(d.x, d.y));
}