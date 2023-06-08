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

    float4 Color : COLOR;
    float4 Emission : EMISSION;
    float2 UV : TEXCOORD;
};


VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;
    Out.UV = In.UV;

    Out.Pos = mul(In.Pos, In.matWVP);

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
    float2 noiseSpeed = float2(0.8f, 0.5f);
    float2 tao = In.UV + noiseSpeed * time;

    float2 noise = Unity_SimpleNoise_float(In.UV + noiseSpeed * time, 25.f);

    float2 lerpf = lerp(In.UV, noise, 0.2f);

    float rect = Unity_Rectangle_float(lerpf, 0.05f, 1.f);



    PSOut Out = (PSOut)0.f;
    Out.Color = In.Color * rect;
    Out.Emission = In.Emission * rect;

    if (Out.Color.w == 0.f)
        discard;

    return Out;
}