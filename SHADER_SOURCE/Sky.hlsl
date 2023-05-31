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

inline float2 unity_voronoi_noise_randomVector(float2 UV, float offset)
{
    float2x2 m = float2x2(15.27, 47.63, 99.41, 89.98);
    UV = frac(sin(mul(UV, m)) * 46839.32);
    return float2(sin(UV.y * +offset) * 0.5 + 0.5, cos(UV.x * offset) * 0.5 + 0.5);
}

void Unity_Voronoi_float(float2 UV, float AngleOffset, float CellDensity, out float Out, out float Cells)
{
    float2 g = floor(UV * CellDensity);
    float2 f = frac(UV * CellDensity);
    float t = 8.0;
    float3 res = float3(8.0, 0.0, 0.0);

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 lattice = float2(x, y);
            float2 offset = unity_voronoi_noise_randomVector(lattice + g, AngleOffset);
            float d = distance(lattice + offset, f);
            if (d < res.x)
            {
                res = float3(d, offset.x, offset.y);
                Out = res.x;
                Cells = res.y;
            }
        }
    }
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