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
    float4 WorldPos : POSITION;
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

    float2 sourceOffset = g_vec2_0;
    float2 sourceSize = g_vec2_1;
    float2 sourceSheetSize = g_vec2_2;
    float2 targetOffset = g_vec2_3;
    float4 Color = g_vec4_0;
    uint renderPosfixed = g_int_0;

    if (sourceSheetSize.x > 0)
        Out.UV = (sourceOffset + In.UV * sourceSize) / sourceSheetSize;
    else
        Out.UV = In.UV;

    if (sourceSheetSize.x > 0) {
        In.Pos.xy = (In.Pos.xy * sourceSize + targetOffset);
    }

    float4 worldPosition = mul(In.Pos, world);
    float4 ProjPosition = (float4)0.f;
    if (renderPosfixed == 1) {
        ProjPosition = mul(worldPosition, projection);
    }
    else
    {
        float4 viewPosition = mul(worldPosition, view);
        ProjPosition = mul(viewPosition, projection);
    }

    Out.Pos = ProjPosition;

    Out.Color = Color;
    Out.WorldPos = worldPosition;
    return Out;
}

PSOut PS_MAIN(VSOut In)
{
    float4 color = (float)0.0f;
    if (g_vec2_2.x > 0)
        color = tex_0.Sample(pointSampler, In.UV);
    else
        color = In.Color;

    for (int i = 0; i < g_lightCount; i++)
        color = color + color * CalculateLight(In.WorldPos.xyz, i);

    if (color.w == 0.f)
        discard;

    PSOut Out = (PSOut)0.f;
    Out.color = color;
    Out.entity = entity;
    return Out;
}