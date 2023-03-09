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

    float2 WorldPos : POSITION;
};

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.0f;

    float2 worldPos = float2(0.0f, 0.0f);
    worldPos.x = In.Pos.x * cameraScale.x * resolution.x + cameraPosition.x;
    worldPos.y = In.Pos.y * cameraScale.y * resolution.y + cameraPosition.y;

    const float meshScale = 2.0f;
    Out.Pos = float4(In.Pos.xy * meshScale, 0.999f, 1.0f);
    Out.UV = In.UV;
    Out.WorldPos = worldPos;

    return Out;
}

float4 PS_MAIN(VSOut In) : SV_TARGET
{
    float4 Out = float4(1.0f, 1.0f, 1.0f, 1.0f);

    const int width = 100;
    const int height = 100;

    uint worldX = (int)In.WorldPos.x;
    uint worldY = (int)In.WorldPos.y;

    // ¼± µÎ²²
    const float thickness = 1.0f;

    if (abs((worldX + 1) % width) <= thickness)
        return Out;

    if (abs((worldY + 1) % height) <= thickness)
        return Out;

    discard;
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}