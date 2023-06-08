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

    float4 Color = g_vec4_0;

    Out.UV = In.UV;

    In.Pos.xy *= viewPort;

    float4 worldPosition = mul(In.Pos, world);
    float4 projPosition = mul(worldPosition, projection);

    Out.Pos = projPosition;
    Out.UV = In.UV;

    return Out;
}

float4 PS_MAIN(VSOut In) : SV_Target0
{
    float Out;
    float Cells;

    Unity_Voronoi_float((In.UV + float2(time / 5.f, 0.f)), time * 0.75, 10, Out, Cells);

    float dis = pow(Out, 2);
    dis = dis * (1 - In.UV.y) * tex_0.Sample(pointSampler, In.UV + time / 5.f).x;

    return float4(1, 0.25, 0.25, dis * 2.f);
}