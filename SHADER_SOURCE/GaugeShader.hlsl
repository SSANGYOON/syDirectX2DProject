#include "global.hlsli"

struct VSIn
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float4 Color : COLOR;
    float2 barUV : TEXCOORD0;
    float2 gaugeUV : TEXCOORD1;
};

cbuffer barBuffer : register(b5)
{
    float2 barSize;
    float2 gaugeSize;
    float curValue;
    float maxVaule;
    float2 paddingbar;
}

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;

    In.Pos.xy = In.Pos.xy * barSize;
    float4 worldPosition = mul(In.Pos, world);
    float4 ProjPosition = mul(worldPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.barUV = In.UV;
    Out.gaugeUV = (In.UV - 0.5f) * barSize / gaugeSize + 0.5f;

    return Out;
}

float4 PS_MAIN(VSOut In) : SV_Target0
{
    float frac = curValue / maxVaule;

    float4 color = (float)0.0f;

    color = tex_0.Sample(anisotropicSampler, In.barUV);


    if(In.gaugeUV.x < frac  && In.gaugeUV.x > 0.f && In.gaugeUV.y > 0.f && In.gaugeUV.y < 1.f)
        color = tex_1.Sample(anisotropicSampler, In.gaugeUV);

    if (color.w == 0.f)
        discard;

    return color;
}
