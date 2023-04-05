#include "global.hlsli"

struct VS_IN
{
    float4 Pos : POSITION;
    uint id : SV_InstanceID;
};

struct VS_OUT
{
    float4 Pos : SV_Position;
    uint id : SV_InstanceID;
};


struct Particle
{
    float3  worldPos;
    float   curTime;
    float3  worldDir;
    float   lifeTime;
    int     alive;
    float3  padding;
};

StructuredBuffer<Particle> g_data : register(t4);

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT Out = (VS_OUT)0.0f;
    float4 worldPos = mul(input.Pos + float4(g_data[input.id].worldPos, 0.f), world);


    Out.Pos = mul(worldPos, view);
    Out.id = input.id;

    return Out;
}

struct GS_OUT
{
    float4 Pos : SV_Position;
    float2 uv : TEXCOORD;
    uint id : SV_InstanceID;
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

    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float2 dir = g_data[id].worldDir.xy;
    float2 scale = float2(g_vec2_0.x /10, (g_vec2_0.y + g_vec2_0.y * ratio) / 10);

    output[0].Pos = vtx.Pos;
    output[1].Pos = vtx.Pos;
    output[2].Pos = vtx.Pos;
    output[3].Pos = vtx.Pos;

    output[0].Pos.xy = vtx.Pos.xy + (dir * scale.y - float2(dir.y, -dir.x) * scale.x) /2.f;
    output[1].Pos.xy = vtx.Pos.xy + (dir * scale.y + float2(dir.y, -dir.x) * scale.x) / 2.f;
    output[2].Pos.xy = vtx.Pos.xy + (-dir * scale.y + float2(dir.y, -dir.x) * scale.x) / 2.f;
    output[3].Pos.xy = vtx.Pos.xy + (-dir * scale.y - float2(dir.y, -dir.x) * scale.x) / 2.f;


    output[0].Pos = mul(output[0].Pos, projection);
    output[1].Pos = mul(output[1].Pos, projection);
    output[2].Pos = mul(output[2].Pos, projection);
    output[3].Pos = mul(output[3].Pos, projection);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

float4 PS_MAIN(GS_OUT input) : SV_Target
{
    float ratio = 1.f - g_data[input.id].curTime / g_data[input.id].lifeTime;
    float4 color = tex_0.Sample(pointSampler, input.uv);

    color.w *= ratio;

    if (color.w == 0)
        discard;
    return color;
}