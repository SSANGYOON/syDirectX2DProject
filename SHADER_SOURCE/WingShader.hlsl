#include "global.hlsli"

struct VSIn
{
    // VertexData
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;

    //InstanceData

    float4 Color : COLOR;
    float4 Emission : EMISSION;

    row_major matrix matW : W;
    row_major matrix matWVP : WVP;

    float2 Tile : TILE;
    float2 Offset : OFFSET;

    uint instanceID : SV_InstanceID;
};

struct VSOut
{
    float4 Pos : SV_Position;
    float3 WorldPos : Position;

    float4 Color : COLOR;
    float4 Emission : EMISSION;
    float2 UV : TEXCOORD;
};


// g_float_0 waveLength
// g_float_1 cycle
// g_vec2_0 rectSize
// g_vec4_0 color

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;
    Out.UV = In.UV;

    In.Pos.x += 0.5f;

    Out.Pos = mul(In.Pos, In.matWVP);
    Out.WorldPos = mul(In.Pos, In.matW).xyz;

    Out.Color = In.Color;
    Out.Emission = In.Emission;

    return Out;
}

struct PSOut
{
    float4 Color : SV_Target0;
    float4 Emission : SV_Target1;
};

PSOut PS_MAIN(VSOut In)
{
    const float pi = 3.14159265359;

    float thickness = g_float_0;
    float amplitude = g_float_1;

    float waveLength = g_float_2;  
    float cycle = g_float_3;

    float div = (In.UV.x * 0.5f * sin(In.UV.x / waveLength * 2 * pi - time / cycle * 2 * pi) + 0.5f) * amplitude;

    if (abs(In.UV.y - div) > (1 - In.UV.x) * thickness * 0.5f)
        discard;

    PSOut Out = (PSOut)0.f;
    Out.Color = In.Color;
    Out.Emission = In.Emission;
    
    return Out;
}