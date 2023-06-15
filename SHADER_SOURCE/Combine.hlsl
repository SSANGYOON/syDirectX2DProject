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
    float4 origin = tex_0.Sample(pointSampler, In.UV);
    float4 blur = tex_1.Sample(linearSampler, In.UV + 1.f / ImgSize);
    float4 hdr = origin + bloomIntensity * blur;
    hdr.xyz = hdr * (1 - FadeColor.w) + FadeColor.w * FadeColor.xyz;

    float x = In.Pos.x / viewPort.x;
    float y = In.Pos.y / viewPort.y;

    if (x < ScissorRect.x || ScissorRect.x + ScissorRect.z < x)
        return float4(0,0,0,1);

    if (y < ScissorRect.y || ScissorRect.y + ScissorRect.w < y)
        return float4(0, 0, 0, 1);

    return hdr;
}