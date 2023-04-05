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
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
};

// g_float_0 waveLength
// g_float_1 cycle
// g_vec2_0 rectSize
// g_vec4_0 color

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;

    In.Pos.xy = In.Pos.xy * g_vec2_0 / 10.f;

    float4 worldPosition = mul(In.Pos, world);
    float4 viewPosition = mul(worldPosition, view);
    float4 ProjPosition = mul(viewPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.UV = In.UV;

    return Out;
}


float4 PS_MAIN(VSOut In) : SV_TARGET
{
    const float pi = 3.14159265359;
    float amp = In.UV.x - 0.5f;
    float waveLength = g_float_0;
    float div = amp * 0.5f * sin(amp * g_vec2_0.x / waveLength * pi + g_float_2 / g_float_1 * 2 * pi) + 0.5;

    if (abs(In.UV.y - div) > (0.5 - abs(amp)) * g_float_3 / g_vec2_0.y || amp > 0)
        discard;

    return g_vec4_0;
}