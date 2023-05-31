#include "global.hlsli"

struct VSIn
{
    // VertexData
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;

    //InstanceData

    float4 Color : COLOR;
    float4 Emission : EMISSION;

    row_major matrix matW : W;
    row_major matrix matWVP : WVP;

    float2 Tile : TILE;
    float2 Offset : OFFSET;

    uint instanceID : SV_InstanceID;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float3 WorldPos : Position;

    float4 Color : COLOR;
    float4 Emission : EMISSION;
    float2 UV : TEXCOORD;
};


VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;
    Out.UV = In.UV * In.Tile + In.Offset;

    if (tex0_On)
        In.Pos.xy *= (g_tex0_size * In.Tile);

    Out.Pos = mul(In.Pos, In.matWVP);
    Out.WorldPos = mul(In.Pos, In.matW).xyz;

    Out.Color = In.Color;
    Out.Emission = In.Emission;

    return Out;
}

struct PSOut
{
    float4 Color : SV_Target0;
    float4 Emission : SV_Target1;
};

PSOut PS_MAIN(VSOut In)
{
    float4 color = In.Color;

    if (tex0_On == 1)
        color *= tex_0.Sample(pointSampler, In.UV);

    if (color.w == 0.f)
        discard;

    PSOut Out = (PSOut)0.f;
    color.w *= (1 -  2 * abs(0.5f - In.UV.x));
    Out.Color = color;

    return Out;
}