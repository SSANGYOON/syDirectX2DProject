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

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;

    float4 pos = (float4)0.f;

    pos.xy = In.UV*2.f -1.f;
    pos.z = 0.f;
    pos.w = 1.f;

    Out.Pos.xy = In.Pos.xy * 2;
    Out.Pos.zw = float2(0.5f, 1.f);
    Out.Color = In.Color;
    Out.UV = In.UV;

    return Out;
}

float4 PS_MAIN(VSOut In) : SV_TARGET
{

    float4 color = (float)1.0f;
    color = tex_0.Sample(anisotropicSampler, In.UV);
    if (color.y < 0.99f)
        discard;

    return color;
}