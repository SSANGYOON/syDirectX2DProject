cbuffer Transform : register(b0)
{
    row_major matrix world;
    row_major matrix view;
    row_major matrix projection;
}

cbuffer Sprite : register(b1)
{
    float2 sourceOffset;
    float2 sourceSize;
    float2 sourceSheetSize;
    float2 targetOffset;
    float2 targetSizeRatio;
}

cbuffer MaterialData : register(b2)
{
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
    float4  g_vec4_0;
    float4  g_vec4_1;
    float4  g_vec4_2;
    float4  g_vec4_3;
    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
    row_major float4x4 g_mat_2;
    row_major float4x4 g_mat_3;
}

struct LightInfo
{
    float3 dir;
    float angle;
    float3 position;
    float range;
    float4 color;
    uint type;
    float3 padding;
};

cbuffer Lights : register(b3)
{
    int         g_lightCount;
    float3      g_lightPadding;
    LightInfo   g_light[50];
}

SamplerState pointSampler : register(s0);
SamplerState linearSampler : register(s1);
SamplerState anisotropicSampler : register(s2);

Texture2D tex_0 : register(t0);
Texture2D tex_1 : register(t1);
Texture2D tex_2 : register(t2);
Texture2D tex_3 : register(t3);

float Rand(float2 co)
{
    return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}

float4 CalculateLight(float3 position, int idx)
{
    if (0 == g_light[idx].type)
    {
        float dis = distance(position, g_light[idx].position);
        
        if (dis > g_light[idx].range)
            return (float4)0;
        else
            return g_light[idx].color *(1 - dis / g_light[idx].range);
    }
    else if (1 == g_light[idx].type)
    {
        float3 diff = position - g_light[idx].position;

        float proj = dot(diff, g_light[idx].dir);

        if (proj < 0 || proj < length(diff) * cos(g_light[idx].angle) || proj > g_light[idx].range)
            return (float4)0;
        else
            return g_light[idx].color * (1 - proj / g_light[idx].range);
    }
    else if (2 == g_light[idx].type)
    {
        return g_light[idx].color;
    }
    else
    {
        float3 diff = position - g_light[idx].position;

        if (length(diff) > g_light[idx].range || g_light[idx].position.z > position.z)
            return (float4)0;
        else
            return g_light[idx].color * length(diff) / g_light[idx].range;
    }
}