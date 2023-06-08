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

    float flip : Flip;
};


VSOut VS_MAIN(VSIn In)
{
    VSOut Out = (VSOut)0.f;
    Out.UV = In.UV * In.Tile + In.Offset;

    if (tex0_On)
        In.Pos.xy *= (g_tex0_size * In.Tile);

    Out.Pos = mul(In.Pos, In.matWVP);
    Out.WorldPos = mul(In.Pos, In.matW).xyz;

    Out.Color = In.Color;
    Out.Emission = In.Emission;
    if (In.matW[0][0] > 0)
        Out.flip = 1.f;
    else
        Out.flip = -1.f;

    return Out;
}

struct PSOut
{
    float4 Color : SV_Target0;
    float4 Emission : SV_Target1;
};

PSOut PS_MAIN(VSOut In)
{
    int eraserType = g_int_0;

    // 0 Nothing
    // 1 Left
    // 2 right
    // 3 up
    // 4 down
    // 5 distance closer
    // 6 distance futher

    float4 worldPoint = float4(g_vec2_0, 0, 1);

    if(eraserType == 1 && worldPoint.x < In.WorldPos.x)
        discard;
    if (eraserType == 2 && worldPoint.x > In.WorldPos.x)
        discard;
    if (eraserType == 3 && worldPoint.y > In.WorldPos.y)
        discard;
    if (eraserType == 4 && worldPoint.y < In.WorldPos.y)
        discard;
    if (eraserType == 5 && distance(worldPoint.xy, In.WorldPos.xy) > g_float_0)
        discard;
    if (eraserType == 6 && distance(worldPoint.xy, In.WorldPos.xy) < g_float_0)
        discard;

    PSOut Out = (PSOut)0.f;
    float4 color = In.Color;

    if (tex0_On == 1)
        color *= tex_0.Sample(pointSampler, In.UV);

    float2 rim = float2(0, 0);
    float addedAlpha = 0;


    float2 aux = 1 / g_tex0_size;
    float value = 0;
    aux.y = 0;
    value = tex_0.Sample(pointSampler, In.UV + aux).a;
    rim.x += In.flip * value;
    addedAlpha += value;
    value = tex_0.Sample(pointSampler, In.UV - aux).a;
    rim.x -= In.flip * value;
    addedAlpha += value;

    aux = 1 / g_tex0_size;
    aux.x = 0;
    value = tex_0.Sample(pointSampler, In.UV - aux).a;
    rim.y += value;
    addedAlpha += value;
    value = tex_0.Sample(pointSampler, In.UV + aux).a;
    rim.y -= value;
    addedAlpha += value;

    float3 rimColor = (float3)0.f;
    float3 ret = (float3)0.f;
    for (uint idx = 0; idx < g_lightCount; idx++) {
        if (addedAlpha < 4) {
            if (0 == g_light[idx].type)
            {
                float dis = distance(In.WorldPos.xy, g_light[idx].position.xy);

                if (dis < g_light[idx].range) {
                    float2 dir = normalize(In.WorldPos.xy - g_light[idx].position.xy);
                    rimColor += g_light[idx].color * pow(1 - dis / g_light[idx].range, 2) * saturate(dot(dir, rim));
                }
            }
            else if (1 == g_light[idx].type)
            {
                float3 diff = In.WorldPos - g_light[idx].position;

                float proj = dot(diff, g_light[idx].dir);

                if (proj > 0 && proj > length(diff.xy) * cos(g_light[idx].angle) && proj < g_light[idx].range) {
                    float2 dir = normalize(diff.xy);
                    rimColor += g_light[idx].color * pow(1 - proj / g_light[idx].range, 2) * saturate(dot(dir, rim.xy));
                }
            }
        }
        if (2 <= g_light[idx].type)
            ret += g_light[idx].color;
    }
    Out.Color = color;

    if (color.w == 0.f)
        discard;

    if (rimColor.x > 0.5f || rimColor.y > 0.5f || rimColor.z > 0.5f) {
        Out.Color.xyz = rimColor;
        Out.Emission.xyz = rimColor;
        Out.Emission.w = 1.f;
    }

    else {
        Out.Color.xyz = ret * color.xyz;
        Out.Emission = In.Emission;
    }
    
    

    return Out;
}