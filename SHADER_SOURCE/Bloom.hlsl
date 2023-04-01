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
    In.Pos.xy = In.Pos.xy * g_vec2_0 / 10.f;
    float4 worldPosition = mul(In.Pos, world);
    float4 viewPosition = mul(worldPosition, view);
    float4 ProjPosition = mul(viewPosition, projection);

    Out.Pos = ProjPosition;
    Out.Color = In.Color;
    Out.UV = In.UV;

    return Out;
}

float4 PS_MAIN(VSOut In) : SV_TARGET
{
    float4 frag = (float)0.0f;

    float alpha = 0;

    const float weight[] = {
      1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
    };

    float2 texel = 1.f / (g_vec2_0);
    float weightSum = 0.f;
    float4 color = tex_0.Sample(pointSampler, In.UV);
    for (int i = -6; i <= 6; i++)
    {
        for (int j = -6; j <= 6; j++)
        {
            float2 diff = float2(texel.x * i, texel.y * j);
            frag += weight[abs(i)] * weight[abs(j)] * tex_0.Sample(anisotropicSampler, In.UV + diff);
            weightSum += weight[abs(i)] * weight[abs(j)];
        }
    }
    color = pow(pow(abs(color), 2.2f) +pow(abs(frag * 1.5f / weightSum), 2.2f), 1/2.2f);
    color.xyz = float3(153.f/255.f, 217.f, 234.f / 255.f);

    return color;
}