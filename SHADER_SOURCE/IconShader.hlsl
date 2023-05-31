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

    float4 worldPosition = mul(In.Pos, world);
    float4 ProjPosition = mul(worldPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.UV = In.UV;

    return Out;
}

float4 PS_MAIN(VSOut In) : SV_Target0
{
    float4 color = (float)0.0f;
    color = tex_0.Sample(linearSampler, In.UV);
    color *= g_vec4_0;
    if (color.w == 0.f)
        discard;

    return color;
}