#include "global.hlsli"

struct VSIn
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float4 Color : COLOR;
    float2 slotUV : TEXCOORD0;
    float2 itemUV : TEXCOORD1;
};

VSOut VS_MAIN(VSIn In)
{
    float itemRotation = g_float_0;
    float2 slotSize = g_vec2_0;
    float2 itemSize = g_vec2_1;
    float2 itemMagnification = g_vec2_2;

    VSOut Out = (VSOut)0.f;

    In.Pos.xy = In.Pos.xy;
    float4 worldPosition = mul(In.Pos, world);
    float4 ProjPosition = mul(worldPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.slotUV = In.UV;

    float2 gaugeUV = (In.UV - 0.5f);
    gaugeUV.y *= -1.f;
    float cosine = cos(itemRotation);
    float sine = sin(itemRotation);

    Out.itemUV.x = gaugeUV.x * cosine + gaugeUV.y * sine;
    Out.itemUV.y = -1.f * (-gaugeUV.x * sine + gaugeUV.y * cosine);
    Out.itemUV *= slotSize / (itemSize * itemMagnification);
    Out.itemUV += 0.5f;
 
    return Out;
}

float4 PS_MAIN(VSOut In) : SV_Target0
{
    float4 color = (float)0.0f;

    color = tex_0.Sample(anisotropicSampler, In.slotUV);


    if (In.itemUV.x < 1.f && In.itemUV.x > 0.f && In.itemUV.y > 0.f && In.itemUV.y < 1.f) {
        float4 itemcolor = tex_1.Sample(anisotropicSampler, In.itemUV) * tex_2.Sample(anisotropicSampler, In.slotUV);

        if (itemcolor.w > 0.f)
            color = itemcolor;
    }

    if (color.w == 0.f)
        discard;

    return color;
}
