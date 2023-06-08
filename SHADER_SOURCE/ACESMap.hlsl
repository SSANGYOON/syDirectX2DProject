#include "global.hlsli"

struct VSIn
{
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD;
};

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;

    Out.Pos = In.Pos * 2.f;

    Out.Pos.w = 1.f;
    Out.UV = In.UV;

    return Out;
}

float3 ToneMapACESFilmic(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 PS_MAIN(VSOut In) : SV_TARGET0
{
    float4 hdr = tex_0.Sample(pointSampler, In.UV);
    hdr.xyz = ToneMapACESFilmic(hdr.xyz);;

    return hdr;
}