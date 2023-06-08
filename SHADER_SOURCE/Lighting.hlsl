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
    float2 UV : TEXCOORD;
};


VSOut VS_MAIN(VSIn In)
{
    VSOut output = (VSOut)0;

    Out.Pos = ProjPosition;
    Out.UV = In.UV;

    return output;
}

float4 PS_MAIN(VSOut In) : SV_Target0
{
    float4 diffuse = tex_0.Sample(pointSampler, In.UV);
    float4 Pos = tex_1.Sample(pointSampler, In.UV);
    float gain = tex_2.Sample(pointSampler, In.UV).x;

    if(gain == 0)
        gain = 1.f;

    diffuse.xyz *= CalculateLight(Pos.xyz, gain);

    return diffuse;
}