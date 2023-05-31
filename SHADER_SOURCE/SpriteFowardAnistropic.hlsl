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

VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;
    Out.UV = In.UV;

    Out.Pos = mul(In.Pos, In.matWVP);
    Out.WorldPos = mul(In.Pos, In.matW).xyz;

    Out.Color = In.Color;
    Out.Emission = In.Emission;

    return Out;
}

struct GS_OUT
{
    float4 Pos : SV_Position;
    float2 uv : TEXCOORD0;
    uint id : SV_InstanceID;
};

[maxvertexcount(1200)]
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
    float2x2 mat = { rot, float2(-rot.y, rot.x) };

    output[0].Pos.xy += mul(size * float2(-0.5f, 0.5f), mat);
    output[1].Pos.xy += mul(size * float2(0.5f, 0.5f), mat);
    output[2].Pos.xy += mul(size * float2(-0.5f, -0.5f), mat);
    output[3].Pos.xy += mul(size * float2(0.5f, -0.5f), mat);

    output[0].Pos = mul(mul(output[0].Pos, view), projection);
    output[1].Pos = mul(mul(output[1].Pos, view), projection);
    output[2].Pos = mul(mul(output[2].Pos, view), projection);
    output[3].Pos = mul(mul(output[3].Pos, view), projection);

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

struct PSOut
{
    float4 Color : SV_Target0;
    float4 Emission : SV_Target1;
};

PSOut PS_MAIN(VSOut In)
{
    float4 color = In.Color;

    if (tex0_On == 1)
        color *= tex_0.Sample(linearSampler, In.UV);

    color.xyz *= CalculateLight(In.WorldPos, 1.f);

    if (color.w == 0.f)
        discard;

    PSOut Out = (PSOut)0.f;
    Out.Color = color;
    Out.Emission = In.Emission;

    return Out;
}