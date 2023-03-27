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

    Out.UV.x = (g_int_0 - In.UV.x + g_int_1) / g_int_1;
    Out.UV.y = In.UV.y;

    float4 viewPosition = mul(In.Pos, view);
    float4 ProjPosition = mul(viewPosition, projection);

    Out.Pos = ProjPosition;
    return Out;
}

float4 PS_MAIN(VSOut In) : SV_TARGET
{
    float4 color = (float)0.0f;

    float4 alpha = tex_0.Sample(anisotropicSampler, In.UV);

    color.x = 65.0 / 255.0;
    color.y = 249.0 / 255.0;
    color.z = 45.0 / 255.0;
    color.w = 1.f;

    color.w = alpha.z;
    return color;
}