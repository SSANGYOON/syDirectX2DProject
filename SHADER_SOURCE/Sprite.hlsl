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

struct PSOut
{
    float4 color : SV_Target0;
    int entity : SV_Target1;
};

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;

    if (sourceSheetSize.x > 0)
        Out.UV = (sourceOffset + In.UV * sourceSize) / sourceSheetSize;
    else
        Out.UV = In.UV;

    if (sourceSheetSize.x > 0)
        In.Pos.xy = (In.Pos.xy * sourceSize + targetOffset);

    float4 worldPosition = mul(In.Pos, world);
    float4 viewPosition = mul(worldPosition, view);
    float4 ProjPosition = mul(viewPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.WorldPos = worldPosition.xyz;
    return Out;
}

PSOut PS_MAIN(VSOut In)
{

    float4 color = (float)0.0f;
    if (textured > 0)
        color = tex_0.Sample(pointSampler, In.UV);
    else
        color = spritecolor;

    for (int i = 0; i < g_lightCount; i++)
        color = color + color * CalculateLight(In.WorldPos, i);

    if (color.w == 0.f)
        discard;

    PSOut Out = (PSOut)0.f;
    Out.color = color;
    Out.entity = entity;
    return Out;
}