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
    float2 UV : TEXCOORD;
};

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;    

    float4 viewPosition = mul(In.Pos, view);
    float4 ProjPosition = mul(viewPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = g_vec4_0;
    Out.UV = In.UV;

    return Out;
}

struct PSOut
{
    float4 Color : SV_Target0;
    float4 Emission : SV_Target1;
};

PSOut PS_MAIN(VSOut In)
{

    float alpha = tex_0.Sample(anisotropicSampler, In.UV).x;
    PSOut Out = (PSOut)0.f;
    Out.Color = In.Color;
    Out.Color.w *= alpha;

    Out.Emission = Out.Color;

    return Out;
}