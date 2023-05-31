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
                ret += g_light[idx].color * (1 - dis / g_light[idx].range);
        }
        else if (1 == g_light[idx].type)
        {
            float3 diff = position - g_light[idx].position;

            float proj = dot(diff, g_light[idx].dir);

            if (proj > 0 && proj > length(diff) * cos(g_light[idx].angle) && proj < g_light[idx].range)
                ret += g_light[idx].color * (1 - proj / g_light[idx].range);
        }
        else
        {
            ret += g_light[idx].color;
        }
    }

    return ret * lightmap;
}