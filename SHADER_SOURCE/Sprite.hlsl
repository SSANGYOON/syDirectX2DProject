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

    Out.UV = (sourceOffset + In.UV * sourceSize) / sourceSheetSize;


    pos.xy = In.Pos.xy * sourceSize * targetSizeRatio + targetOffset;
    pos.z = In.Pos.z;
    pos.w = 1.f;

    float4 worldPosition = mul(pos, world);
    float4 viewPosition = mul(worldPosition, view);
    float4 ProjPosition = mul(viewPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    

    return Out;
}

float4 PS_MAIN(VSOut In) : SV_TARGET
{

    float4 color = (float)0.0f;
    color = tex_0.Sample(anisotropicSampler, In.UV);
    if (color.w == 0.f)
        discard;
    return color;
}