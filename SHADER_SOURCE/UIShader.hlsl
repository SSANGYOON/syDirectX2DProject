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
};

struct GSOut
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD;
};

struct PSOut
{
    float4 color : SV_Target0;
    int entity : SV_Target1;
};

VSOut VS_MAIN(VSIn In)
{
    float4 worldPosition = mul(In.Pos, world);

    VSOut output = (VSOut)0.f;
    output.Pos = worldPosition;

    return output;
}

[maxvertexcount(54)]
void GS_MAIN(point VSOut viewPos[1], inout TriangleStream<GSOut> outputStream)
{
    GSOut output[16] = { (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f,
                        (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f, (GSOut)0.f };

    for (uint i = 0; i < 16; i++)
    {
        uint x = i % 4;
        uint y = i / 4;

        output[i].Pos = viewPos[0].Pos;

        float2 size = mul(float4(1.f, 1.f, 0.f, 0.f), world).xy;
        float2 originalSize = g_vec2_0;
        float2 offset = g_vec2_1;


        float right = 1.f;
        if (x < 2)
            right = 0.f;

        float stretch = 0.f;
        if (x > 0 && x < 3)
            stretch = 1.f;

        output[i].Pos.x += (right - 0.5f) * (size.x - stretch * offset.x);
        output[i].UV.x = right - (2 * right - 1.f) * (offset.x * stretch / originalSize.x);
        
        float down = 1.f;
        if (y < 2)
            down = 0.f;

        stretch = 0.f;
        if (y > 0 && y < 3)
            stretch = 1.f;

        output[i].Pos.y += -(down - 0.5f) * (size.y - stretch * offset.y);
        output[i].UV.y = down - (2 * down - 1.f) * (offset.x * stretch / originalSize.x);

        output[i].Pos = mul(output[i].Pos, projection);
    }

    for (uint i = 0; i < 3; i++)
    {
        for (uint j = 0; j < 3; j++)
        {
            outputStream.Append(output[4 * i + j]);
            outputStream.Append(output[4 * i + j + 1]);
            outputStream.Append(output[4 * (i + 1) + j]);
            outputStream.RestartStrip();

            outputStream.Append(output[4 * i + j + 1]);
            outputStream.Append(output[4 * (i + 1) + j]);
            outputStream.Append(output[4 * (i + 1) + j + 1]);
            outputStream.RestartStrip();
        }
    }
}
PSOut PS_MAIN(GSOut In)
{
    float2 originalSize = g_vec2_0;
    float2 offset = g_vec2_1;
    float2 tintRange = g_vec2_2 / g_vec2_0;
    float4 tintColor = g_vec4_0;

    float4 color = tex_0.Sample(linearSampler, In.UV);

    PSOut Out = (PSOut)0.f;
    if (color.w > 0.5f)
        Out.color = color;
    else if(In.UV.x > tintRange.x  && In.UV.x < 1.f - tintRange.x && In.UV.y > tintRange.y && In.UV.y < 1.f - tintRange.y)
        Out.color = tintColor;
    Out.entity = entity;
    return Out;
}