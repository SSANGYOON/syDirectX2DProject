#include "global.hlsli"

struct VSIn
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float3 WorldPos : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

// centerPos g_vec4_0
// color g_vec4_1
// radius g_float_1
// alpha g_float_0

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;

    In.Pos.xy = (In.Pos.xy) * g_float_1 /10.f;

    float4 worldPosition = mul(In.Pos, world);
    float4 viewPosition = mul(worldPosition, view);
    float4 ProjPosition = mul(viewPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.WorldPos = worldPosition.xyz;
    return Out;
}


float4 PS_MAIN(VSOut In) : SV_TARGET
{
    float dis = distance(In.WorldPos, g_vec4_0.xyz);
    float4 color = g_vec4_1;

    color.a = color.a * g_float_0 * (g_float_1 / 10.f - dis) / (g_float_1 /10.f);

    return color;
}