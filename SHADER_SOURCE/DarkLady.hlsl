#include "global.hlsli"

struct VSIn
{
    // VertexData
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;

    //InstanceData
    row_major matrix matW : W;
    row_major matrix matWVP : WVP;

    float2 Tile : TILE;
    float2 Offset : OFFSET;

    float4 Color : COLOR;
    float4 Emission : EMISSION;

    uint instanceID : SV_InstanceID;
};
struct VSOut
{
    float4 Pos : SV_Position;
    float4 WorldPos : Position;

    float4 Color : COLOR;
    float4 Emission : EMISSION;
    float2 UV : TEXCOORD;
};


struct PSOut
{
    float4 Diffuse : SV_Target0;
    float4 Emission : SV_Target1;
    float4 Position : SV_Target2;
    float LightMap : SV_Target3;

};

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;
    Out.UV = In.UV * In.Tile + In.Offset;

    if (tex0_On)
        In.Pos.xy *= (g_tex0_size * In.Tile);

    Out.Pos = mul(In.Pos, In.matWVP);
    Out.WorldPos = mul(In.Pos, In.matW);

    Out.Color = In.Color;
    Out.Emission = In.Emission;

    return Out;
}

PSOut PS_MAIN(VSOut In)
{
    PSOut Out = (PSOut)0.f;

    float4 diffuse = In.Color;
    float Light = 1.f;

    if (tex0_On == 1)
        diffuse *= tex_0.Sample(pointSampler, In.UV);
    if (tex1_On == 1)
        Light = tex_1.Sample(pointSampler, In.UV).x;
    else
        Light = 1.f;

    float alpha = 1;
    if (g_float_0 < distance(In.WorldPos.xy, g_vec2_0))
        alpha = 1;
    else
        alpha = 0;

    Out.Diffuse = diffuse;
    Out.Diffuse.w *= alpha;
    if (Out.Diffuse.w == 0.f)
        discard;

    Out.Position = In.WorldPos;
    Out.LightMap = Light;

    Out.Emission = In.Emission;

    return Out;
}