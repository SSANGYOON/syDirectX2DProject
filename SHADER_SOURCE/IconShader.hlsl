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

struct PSOut
{
    float4 color : SV_Target0;
    int entity : SV_Target1;
};

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;

    float4 worldPosition = mul(In.Pos, world);
    float4 ProjPosition = mul(worldPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.UV = In.UV;

    return Out;
}

PSOut PS_MAIN(VSOut In)
{
    float4 color = (float)0.0f;
    color = tex_0.Sample(linearSampler, In.UV);
    if (color.w == 0.f)
        discard;
    color *= g_vec4_0;

    PSOut Out = (PSOut)0.f;
    Out.color = color;
    Out.entity = entity;
    return Out;
}