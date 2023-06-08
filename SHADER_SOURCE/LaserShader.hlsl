#include "global.hlsli"

struct VSIn
{
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float4 Color : COLOR;
    float4 Emission : EMISSION;
    float2 UV : TEXCOORD;
};

struct PSOut
{
    float4 Color : SV_Target0;
    float4 Emission : SV_Target1;
};

VSOut VS_MAIN(VSIn In)
{
    In.Pos.xy = (In.Pos.xy + float2(0.5f, 0.f)) * g_vec2_1;
    float4 ProjPosition = mul(mul(mul(In.Pos, world), view), projection);

    VSOut Out = (VSOut)0.f;

    Out.Pos = ProjPosition;
    Out.Color = g_vec4_0;
    Out.Emission = g_vec4_1;
    Out.UV = In.UV;

    return Out;
}

PSOut PS_MAIN(VSOut In)
{
    float laser = pow(1 - (pow(1 - In.UV.y,10) + pow(In.UV.y, 10)), 30);

    float voronoi;
    float cells;

    float2 laserVelocity = g_vec2_0;
    Unity_Voronoi_float(In.UV - g_vec2_0 * time, 2, 6, voronoi, cells);

    voronoi = 0.8f * voronoi + 0.2f;

    laser *= voronoi;

    PSOut Out = (PSOut)0.f;
    Out.Color = In.Color;
    Out.Color.w *= laser;
    Out.Emission = In.Emission;
    Out.Emission.w *= laser;

    return Out;
}