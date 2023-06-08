#include "global.hlsli"

struct VS_IN
{
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;

    uint id : SV_InstanceID;
};

struct VS_OUT
{
    float4 Pos : SV_Position;
    uint id : SV_InstanceID;
};


struct Particle
{
    float3 position;
    float remainLife;
    float2 velocity;
    float2 size;
    uint alive;
    float3 initialPos;
};


StructuredBuffer<Particle> g_data : register(t4);

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT Out = (VS_OUT)0.0f;


    Out.Pos = input.Pos;
    Out.id = input.id;

    return Out;
}

struct GS_OUT
{
    float4 Pos : SV_Position;
    float2 uv : TEXCOORD0;
    uint id : SV_InstanceID;
};

struct PSOut
{
    float4 Color : SV_Target0;
    float4 Emission : SV_Target1;
};

[maxvertexcount(6)]
void GS_MAIN(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] =
    {
        (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f
    };

    VS_OUT vtx = input[0];
    uint id = (uint)vtx.id;

    if (0 == g_data[id].alive)
        return;

    float3 pos = g_data[id].position;
    float2 size = g_data[id].size;

    output[0].Pos = float4(pos, 1.f);
    output[1].Pos = float4(pos, 1.f);
    output[2].Pos = float4(pos, 1.f);
    output[3].Pos = float4(pos, 1.f);


    float2 velocity = g_data[id].velocity;

    float2 rot = normalize(float2(velocity.y, -velocity.x));

    if (VelocityPolar > 0)
    {
        float2 posDiff = g_data[id].position.xy - g_data[id].initialPos.xy;
        float2 normal = normalize(posDiff);

        rot = float2(rot.x * normal.x - rot.y * normal.y, rot.x * normal.y + rot.y * normal.x);
    }
    float2x2 mat = { rot, float2(-rot.y, rot.x)};

    output[0].Pos.xy += mul(size *float2(-0.5f, 0.5f), mat);
    output[1].Pos.xy += mul(size *float2(0.5f, 0.5f), mat);
    output[2].Pos.xy += mul(size * float2(-0.5f, -0.5f), mat);
    output[3].Pos.xy += mul(size * float2(0.5f, -0.5f), mat);

    if (useLocalCoord == 1) {
        output[0].Pos = mul(mul(mul(output[0].Pos, world), view), projection);
        output[1].Pos = mul(mul(mul(output[1].Pos, world), view), projection);
        output[2].Pos = mul(mul(mul(output[2].Pos, world), view), projection);
        output[3].Pos = mul(mul(mul(output[3].Pos, world), view), projection);
    }
    else
    {
        output[0].Pos = mul(mul(output[0].Pos, view), projection);
        output[1].Pos = mul(mul(output[1].Pos, view), projection);
        output[2].Pos = mul(mul(output[2].Pos, view), projection);
        output[3].Pos = mul(mul(output[3].Pos, view), projection);
    }

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(0.f, 1.f);
    output[3].uv = float2(1.f, 1.f);

    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

PSOut PS_MAIN(GS_OUT input)
{
    float t = g_data[input.id].remainLife / LifeTime;
    float4 color = lerp(ColorEnd, ColorBegin, t);

    if(tex0_On > 0)
        color *= tex_0.Sample(pointSampler, input.uv);

    float4 Emission = lerp(EmissionEnd, EmissionBegin, t);

    PSOut Out;
    Out.Color = color;
    Out.Emission = Emission;
    
    return Out;
}