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

    if (g_int_2 == 0) {
        Out.UV.x = (g_int_1 - (g_int_0 - In.UV.x)) / g_int_1;
        Out.UV.y = In.UV.y;

        if (Out.UV.x < 0)
            Out.UV.x = 0.f;
    }

    else{
        Out.UV.y = (g_int_1 - (g_int_0 - In.UV.y)) / g_int_1;
        Out.UV.x = In.UV.x;

        if (Out.UV.y < 0)
            Out.UV.y = 0.f;
    }
    

    

    float4 viewPosition = mul(In.Pos, view);
    float4 ProjPosition = mul(viewPosition, projection);

    Out.Pos = ProjPosition;
    return Out;
}

float4 PS_MAIN(VSOut In) : SV_TARGET
{
    float4 color = (float)0.0f;

    float4 alpha = tex_0.Sample(anisotropicSampler, In.UV);

    color.xyz = float3(153.f / 255.f, 217.f, 234.f / 255.f);
    color.w = 1.f;

    color.w = alpha.z;
    return color;
}